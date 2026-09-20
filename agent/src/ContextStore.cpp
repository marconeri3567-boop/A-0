#include "ContextStore.hpp"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

#include <nlohmann/json.hpp>

namespace fs = std::filesystem;
using json = nlohmann::json;

namespace
{
    std::string nowIso8601()
    {
        const auto now = std::chrono::system_clock::now();
        const std::time_t time = std::chrono::system_clock::to_time_t(now);
        std::tm utc{};
#if defined(_WIN32)
        gmtime_s(&utc, &time);
#else
        gmtime_r(&time, &utc);
#endif
        std::ostringstream output;
        output << std::put_time(&utc, "%Y-%m-%dT%H:%M:%SZ");
        return output.str();
    }
}

bool ContextStore::load(const std::string& filePath)
{
    context_ = {};
    std::ifstream input(filePath);
    if (!input.is_open())
    {
        return !fs::exists(filePath);
    }

    try
    {
        const json document = json::parse(input);
        if (!document.is_object()) return false;

        context_.currentInput = document.value("current_input", "");
        context_.currentIntent = document.value("current_intent", "");
        context_.currentConfidence = document.value("current_confidence", 0.0f);

        const auto history = document.value("history", json::array());
        if (!history.is_array()) return false;
        for (const auto& item : history)
        {
            if (!item.is_object()) continue;
            ContextEntry entry;
            entry.input = item.value("input", "");
            entry.intent = item.value("intent", "");
            entry.confidence = item.value("confidence", 0.0f);
            entry.createdAt = item.value("created_at", "");
            if (!entry.input.empty() || !entry.intent.empty())
                context_.history.push_back(std::move(entry));
        }
        return true;
    }
    catch (const std::exception& exception)
    {
        std::cerr << "[ContextStore] Unable to load context: "
                  << exception.what() << '\n';
        return false;
    }
}

bool ContextStore::update(const std::string& input, const std::string& intent,
                          float confidence, const std::string& filePath)
{
    const bool changed = context_.currentInput != input ||
                         context_.currentIntent != intent;
    if (changed)
    {
        context_.history.push_back({input, intent, confidence, nowIso8601()});
        context_.currentInput = input;
        context_.currentIntent = intent;
        context_.currentConfidence = confidence;
    }
    else
    {
        context_.currentConfidence = confidence;
    }

    try
    {
        const fs::path target(filePath);
        if (target.has_parent_path()) fs::create_directories(target.parent_path());
        const fs::path temporary = target.string() + ".tmp";
        json document = {
            {"version", 1},
            {"current_input", context_.currentInput},
            {"current_intent", context_.currentIntent},
            {"current_confidence", context_.currentConfidence},
            {"history", json::array()}
        };
        for (const auto& entry : context_.history)
        {
            document["history"].push_back({
                {"input", entry.input},
                {"intent", entry.intent},
                {"confidence", entry.confidence},
                {"created_at", entry.createdAt}
            });
        }

        std::ofstream output(temporary);
        if (!output.is_open()) return false;
        output << document.dump(2) << '\n';
        output.close();

        std::error_code error;
        fs::rename(temporary, target, error);
        if (error)
        {
            fs::remove(target, error);
            error.clear();
            fs::rename(temporary, target, error);
        }
        if (error)
        {
            fs::remove(temporary);
            return false;
        }
        return true;
    }
    catch (const std::exception& exception)
    {
        std::cerr << "[ContextStore] Unable to save context: "
                  << exception.what() << '\n';
        return false;
    }
}

const ConversationContext& ContextStore::context() const noexcept
{
    return context_;
}

bool ContextStore::hasContext() const noexcept
{
    return !context_.currentInput.empty() || !context_.currentIntent.empty();
}
