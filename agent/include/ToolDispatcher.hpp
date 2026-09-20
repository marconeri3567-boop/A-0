#pragma once

#include "NeuralModel.hpp"

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

struct ToolRequest
{
    std::string intent;
    std::string entity;
    float confidence = 0.0f;
    std::string previousInput;
    std::string previousIntent;
    std::size_t historySize = 0;
};

struct ToolResult
{
    bool success = false;
    std::string message;
};

using ToolHandler = std::function<ToolResult(const ToolRequest&)>;

class ToolDispatcher
{
public:
    ToolDispatcher() = default;
    void registerTool(const std::string& intent, ToolHandler handler);
    [[nodiscard]] bool hasTool(const std::string& intent) const;
    [[nodiscard]] ToolResult dispatch(const ToolRequest& request) const;
    [[nodiscard]] ToolRequest createRequest(
        const PredictionResult& prediction,
        const std::string& previousInput = {},
        const std::string& previousIntent = {},
        std::size_t historySize = 0) const;
    [[nodiscard]] std::size_t size() const noexcept;
    [[nodiscard]] std::vector<std::string> availableTools() const;

private:
    std::unordered_map<std::string, ToolHandler> tools_;
};
