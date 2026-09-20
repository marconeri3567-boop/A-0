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
    if (!file.is_open()) {
        std::cerr << "[DatasetLoader] Unable to open file: " << jsonPath << '\n';
        return false;
    }

    try {
        const json dataset = json::parse(file);
        if (!dataset.is_array()) {
            std::cerr << "[DatasetLoader] Dataset root must be an array.\n";
            return false;
        }
        samples_.reserve(dataset.size());
        for (const auto& item : dataset) {
            if (!item.is_object() || !item.contains("input") || !item.contains("intent") ||
                !item["input"].is_string() || !item["intent"].is_string()) {
                continue;
            }
            const auto input = item["input"].get<std::string>();
            const auto intent = item["intent"].get<std::string>();
            if (validateSample(input, intent)) samples_.push_back({input, intent});
        }
        valid_ = !samples_.empty();
        if (!valid_) std::cerr << "[DatasetLoader] No valid samples found.\n";
        else std::cout << "[DatasetLoader] Loaded " << samples_.size() << " samples.\n";
        return valid_;
    } catch (const std::exception& ex) {
        std::cerr << "[DatasetLoader] JSON parsing error: " << ex.what() << '\n';
        return false;
    }
}

const std::vector<TrainingSample>& DatasetLoader::getSamples() const noexcept { return samples_; }
std::size_t DatasetLoader::size() const noexcept { return samples_.size(); }
bool DatasetLoader::empty() const noexcept { return samples_.empty(); }

std::vector<std::string> DatasetLoader::getUniqueIntents() const
{
    std::unordered_set<std::string> unique;
    for (const auto& sample : samples_) unique.insert(sample.intent);
    return {unique.begin(), unique.end()};
}

bool DatasetLoader::isValid() const noexcept { return valid_; }

bool DatasetLoader::validateSample(const std::string& input, const std::string& intent) const noexcept
{
    return !input.empty() && !intent.empty();
}
