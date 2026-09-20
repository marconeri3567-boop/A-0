#include "DatasetLoader.hpp"

#include <fstream>
#include <iostream>
#include <unordered_set>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

bool DatasetLoader::load(const std::string& jsonPath)
{
    samples_.clear();
    valid_ = false;

    std::ifstream file(jsonPath);

    if (!file.is_open())
    {
        std::cerr
            << "[DatasetLoader] Unable to open file: "
            << jsonPath
            << std::endl;

        return false;
    }

    try
    {
        json dataset;
        file >> dataset;

        if (!dataset.is_array())
        {
            std::cerr
                << "[DatasetLoader] Dataset root must be an array."
                << std::endl;

            return false;
        }

        samples_.reserve(dataset.size());

        for (const auto& item : dataset)
        {
            if (!item.is_object())
            {
                continue;
            }

            if (!item.contains("input"))
            {
                continue;
            }

            if (!item.contains("intent"))
            {
                continue;
            }

            if (!item["input"].is_string())
            {
                continue;
            }

            if (!item["intent"].is_string())
            {
                continue;
            }

            const std::string input =
                item["input"].get<std::string>();

            const std::string intent =
                item["intent"].get<std::string>();

            if (!validateSample(input, intent))
            {
                continue;
            }

            samples_.emplace_back(
                TrainingSample{
                    input,
                    intent
                }
            );
        }

        valid_ = !samples_.empty();

        if (!valid_)
        {
            std::cerr
                << "[DatasetLoader] No valid samples found."
                << std::endl;

            return false;
        }

        std::cout
            << "[DatasetLoader] Loaded "
            << samples_.size()
            << " samples."
            << std::endl;

        return true;
    }
    catch (const std::exception& ex)
    {
        std::cerr
            << "[DatasetLoader] JSON parsing error: "
            << ex.what()
            << std::endl;

        return false;
    }
}

const std::vector<TrainingSample>&
DatasetLoader::getSamples() const noexcept
{
    return samples_;
}

std::size_t DatasetLoader::size() const noexcept
{
    return samples_.size();
}

bool DatasetLoader::empty() const noexcept
{
    return samples_.empty();
}

std::vector<std::string>
DatasetLoader::getUniqueIntents() const
{
    std::unordered_set<std::string> unique;

    unique.reserve(samples_.size());

    for (const auto& sample : samples_)
    {
        unique.insert(sample.intent);
    }

    return {
        unique.begin(),
        unique.end()
    };
}

bool DatasetLoader::isValid() const noexcept
{
    return valid_;
}

bool DatasetLoader::validateSample(
    const std::string& input,
    const std::string& intent
) const noexcept
{
    if (input.empty())
    {
        return false;
    }

    if (intent.empty())
    {
        return false;
    }

    return true;
}
