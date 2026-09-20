#include "Trainer.hpp"
#include "Predictor.hpp"
#include "ToolDispatcher.hpp"
#include "Database.hpp"
#include "ContextStore.hpp"

#include <cstdlib>
#include <iostream>
#include <string>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace
{
    constexpr const char* MODEL_DIR = "models";
    constexpr const char* DB_FILE = "models/local_intent_ai.db";
    constexpr const char* CONTEXT_FILE = "models/context.json";

    void printUsage()
    {
        std::cout
            << "LocalIntentAI\n\n"
            << "Usage:\n"
            << "  local_intent_ai train <dataset.json>\n"
            << "  local_intent_ai predict \"text\"\n"
            << std::endl;
    }

    void registerDefaultTools(ToolDispatcher& dispatcher)
    {
        dispatcher.registerTool("open_browser", [](const ToolRequest&) {
            return ToolResult{true, "Browser tool matched"};
        });
        dispatcher.registerTool("open_editor", [](const ToolRequest&) {
            return ToolResult{true, "Editor tool matched"};
        });
        dispatcher.registerTool("play_music", [](const ToolRequest&) {
            return ToolResult{true, "Music tool matched"};
        });
        dispatcher.registerTool("search_web", [](const ToolRequest&) {
            return ToolResult{true, "Search tool matched"};
        });
    }

    json contextToJson(const ConversationContext& context)
    {
        json history = json::array();
        for (const auto& entry : context.history)
        {
            history.push_back({
                {"input", entry.input},
                {"intent", entry.intent},
                {"confidence", entry.confidence},
                {"created_at", entry.createdAt}
            });
        }
        return {
            {"current_input", context.currentInput},
            {"current_intent", context.currentIntent},
            {"current_confidence", context.currentConfidence},
            {"history", std::move(history)}
        };
    }
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        printUsage();
        return EXIT_FAILURE;
    }

    const std::string command = argv[1];
    Database database;
    if (!database.open(DB_FILE))
        std::cerr << "[Main] Unable to open database." << std::endl;

    if (command == "train")
    {
        if (argc < 3)
        {
            printUsage();
            return EXIT_FAILURE;
        }

        Trainer trainer;
        constexpr std::size_t epochs = 150;
        constexpr std::size_t batchSize = 8;
        const bool success = trainer.train(argv[2], MODEL_DIR, epochs, batchSize);
        if (!success)
        {
            database.insertLog("ERROR", "Training failed");
            return EXIT_FAILURE;
        }

        database.insertTrainingRun(argv[2], epochs, trainer.intentCount(),
                                   trainer.vocabularySize(), trainer.intentCount());
        database.insertLog("INFO", "Training completed");
        std::cout << "\nTraining completed successfully.\n" << std::endl;
        return EXIT_SUCCESS;
    }

    if (command != "predict" || argc < 3)
    {
        printUsage();
        return EXIT_FAILURE;
    }

    const std::string text = argv[2];
    Predictor predictor;
    if (!predictor.load(MODEL_DIR))
    {
        std::cerr << "[Main] Model loading failed." << std::endl;
        return EXIT_FAILURE;
    }

    // Reload persisted state for every request, so separate process invocations
    // continue the same conversation.
    ContextStore contextStore;
    if (!contextStore.load(CONTEXT_FILE))
    {
        database.insertLog("ERROR", "Context history could not be loaded");
        return EXIT_FAILURE;
    }
    const ConversationContext previousContext = contextStore.context();

    const PredictionResult prediction = predictor.predict(text);
    if (prediction.intent.empty())
    {
        database.insertLog("ERROR", "Prediction returned an empty intent");
        return EXIT_FAILURE;
    }

    if (!contextStore.update(text, prediction.intent, prediction.confidence,
                             CONTEXT_FILE))
    {
        database.insertLog("ERROR", "Context history could not be saved");
        return EXIT_FAILURE;
    }

    database.insertPrediction(text, prediction.intent, prediction.confidence);

    ToolDispatcher dispatcher;
    registerDefaultTools(dispatcher);
    const ToolRequest request = dispatcher.createRequest(
        prediction,
        previousContext.currentInput,
        previousContext.currentIntent,
        previousContext.history.size());
    const ToolResult toolResult = dispatcher.dispatch(request);

    json response = {
        {"input", text},
        {"intent", prediction.intent},
        {"confidence", prediction.confidence},
        {"previous_context", {
            {"input", previousContext.currentInput},
            {"intent", previousContext.currentIntent},
            {"confidence", previousContext.currentConfidence},
            {"history_size", previousContext.history.size()}
        }},
        {"context", contextToJson(contextStore.context())},
        {"tool_success", toolResult.success},
        {"tool_message", toolResult.message}
    };

    std::cout << response.dump(2) << std::endl;
    return EXIT_SUCCESS;
}
