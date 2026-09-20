#include "ToolDispatcher.hpp"

#include <exception>
#include <iostream>

void ToolDispatcher::registerTool(
    const std::string& intent,
    ToolHandler handler)
{
    if (intent.empty())
    {
        return;
    }

    if (!handler)
    {
        return;
    }

    tools_[intent] = std::move(handler);
}

bool ToolDispatcher::hasTool(
    const std::string& intent) const
{
    return tools_.find(intent)
           != tools_.end();
}

ToolResult ToolDispatcher::dispatch(
    const ToolRequest& request) const
{
    ToolResult result;

    if (request.intent.empty())
    {
        result.success = false;
        result.message =
            "Empty intent.";

        return result;
    }

    const auto it =
        tools_.find(request.intent);

    if (it == tools_.end())
    {
        result.success = false;
        result.message =
            "No handler registered for intent: "
            + request.intent;

        return result;
    }

    try
    {
        return it->second(request);
    }
    catch (const std::exception& ex)
    {
        result.success = false;
        result.message =
            std::string("Handler exception: ")
            + ex.what();

        return result;
    }
    catch (...)
    {
        result.success = false;
        result.message =
            "Unknown handler exception.";

        return result;
    }
}

ToolRequest ToolDispatcher::createRequest(
    const PredictionResult& prediction) const
{
    ToolRequest request;

    request.intent =
        prediction.intent;

    request.confidence =
        prediction.confidence;

    return request;
}

std::size_t ToolDispatcher::size() const noexcept
{
    return tools_.size();
}

std::vector<std::string>
ToolDispatcher::availableTools() const
{
    std::vector<std::string> intents;

    intents.reserve(
        tools_.size());

    for (const auto& [intent, handler]
         : tools_)
    {
        (void)handler;

        intents.push_back(intent);
    }

    return intents;
}
