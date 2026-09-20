#include "DatasetConverter.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

namespace fs = std::filesystem;
using json = nlohmann::json;

namespace
{
    bool isStringField(
        const json& object,
        const char* field)
    {
        return object.contains(field) && object[field].is_string();
    }

    bool readSample(
        const json& item,
        json& normalized,
        bool& alreadyNormalized)
    {
        if (!item.is_object())
        {
            return false;
        }

        if (isStringField(item, "input") &&
            isStringField(item, "intent"))
        {
            normalized = {
                {"input", item["input"]},
                {"intent", item["intent"]}
            };
            return true;
        }

        if (isStringField(item, "request") &&
            isStringField(item, "answer"))
        {
            alreadyNormalized = false;
            normalized = {
                {"input", item["request"]},
                {"intent", item["answer"]}
            };
            return true;
        }

        return false;
    }
}

bool DatasetConverter::prepare(
    const std::string& inputPath,
    const std::string& outputDir,
    DatasetPreparation& preparation) const
{
    preparation = {};

    std::ifstream input(inputPath);
    if (!input.is_open())
    {
        std::cerr << "[DatasetConverter] Unable to open file: "
                  << inputPath << '\n';
        return false;
    }

    try
    {
        const json dataset = json::parse(input);
        if (!dataset.is_array() || dataset.empty())
        {
            std::cerr << "[DatasetConverter] Dataset must be a non-empty JSON array.\n";
            return false;
        }

        bool alreadyNormalized = true;
        json normalized = json::array();
        normalized.reserve(dataset.size());

        for (const auto& item : dataset)
        {
            json sample;
            if (!readSample(item, sample, alreadyNormalized))
            {
                std::cerr
                    << "[DatasetConverter] Unsupported sample format. "
                    << "Expected input/intent or request/answer.\n";
                return false;
            }
            normalized.push_back(std::move(sample));
        }

        if (alreadyNormalized)
        {
            preparation.path = inputPath;
            preparation.converted = false;
            std::cout << "[DatasetConverter] Dataset already normalized: "
                      << inputPath << '\n';
            return true;
        }

        fs::create_directories(outputDir);
        const fs::path outputPath =
            fs::path(outputDir) / "normalized_dataset.json";
        const fs::path temporaryPath =
            fs::path(outputDir) / "normalized_dataset.json.tmp";

        std::ofstream output(temporaryPath);
        if (!output.is_open())
        {
            std::cerr << "[DatasetConverter] Unable to create normalized dataset: "
                      << outputPath << '\n';
            return false;
        }
        output << normalized.dump(2) << '\n';
        output.close();

        std::error_code error;
        fs::rename(temporaryPath, outputPath, error);
        if (error)
        {
            fs::remove(outputPath, error);
            error.clear();
            fs::rename(temporaryPath, outputPath, error);
        }
        if (error)
        {
            std::cerr << "[DatasetConverter] Unable to finalize normalized dataset: "
                      << error.message() << '\n';
            fs::remove(temporaryPath);
            return false;
        }

        preparation.path = outputPath.string();
        preparation.converted = true;
        std::cout << "[DatasetConverter] Converted dataset to: "
                  << preparation.path << '\n';
        return true;
    }
    catch (const std::exception& exception)
    {
        std::cerr << "[DatasetConverter] JSON conversion error: "
                  << exception.what() << '\n';
        return false;
    }
}
