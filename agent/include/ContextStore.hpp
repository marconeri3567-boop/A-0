#pragma once

#include <cstddef>
#include <string>
#include <vector>

/** A single persisted interaction in the conversation history. */
struct ContextEntry
{
    std::string input;
    std::string intent;
    float confidence = 0.0f;
    std::string createdAt;
};

/** The current context plus its chronological changes. */
struct ConversationContext
{
    std::string currentInput;
    std::string currentIntent;
    float currentConfidence = 0.0f;
    std::vector<ContextEntry> history;
};

/**
 * Persists conversation context as JSON and reloads it for every prediction.
 * A history entry is added only when the effective context changes.
 */
class ContextStore
{
public:
    bool load(const std::string& filePath);
    bool update(const std::string& input, const std::string& intent,
                float confidence, const std::string& filePath);

    [[nodiscard]] const ConversationContext& context() const noexcept;
    [[nodiscard]] bool hasContext() const noexcept;

private:
    ConversationContext context_;
};
