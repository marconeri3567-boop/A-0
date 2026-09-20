#include "ToolDispatcher.hpp"

#include <exception>

void ToolDispatcher::registerTool(const std::string& intent, ToolHandler handler)
{
    if (!intent.empty() && handler) tools_[intent] = std::move(handler);
}

bool ToolDispatcher::hasTool(const std::string& intent) const
{
    return tools_.find(intent) != tools_.end();
}

ToolResult ToolDispatcher::dispatch(const ToolRequest& request) const
{
    if (request.intent.empty()) return {false, "Empty intent."};
    const auto it = tools_.find(request.intent);
    if (it == tools_.end())
        return {false, "No handler registered for intent: " + request.intent};
    try { return it->second(request); }
    catch (const std::exception& exception)
    { return {false, std::string("Handler exception: ") + exception.what()}; }
    catch (...) { return {false, "Unknown handler exception."}; }
}

ToolRequest ToolDispatcher::createRequest(const PredictionResult& prediction,
                                          const std::string& previousInput,
                                          const std::string& previousIntent,
                                          std::size_t historySize) const
{
    return {prediction.intent, {}, prediction.confidence,
            previousInput, previousIntent, historySize};
}

std::size_t ToolDispatcher::size() const noexcept { return tools_.size(); }

std::vector<std::string> ToolDispatcher::availableTools() const
{
    std::vector<std::string> intents;
    intents.reserve(tools_.size());
    for (const auto& [intent, handler] : tools_)
    {
        (void)handler;
        intents.push_back(intent);
    }
    return intents;
}
