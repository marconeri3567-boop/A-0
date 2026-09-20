#include "Trainer.hpp"
#include "Predictor.hpp"
#include "ToolDispatcher.hpp"
#include "Database.hpp"

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>

namespace
{
    constexpr const char* MODEL_DIR = "models";
    constexpr const char* DB_FILE   = "models/local_intent_ai.db";

    void printUsage()
    {
        std::cout
            << "LocalIntentAI\n\n"
            << "Usage:\n"
            << "  local_intent_ai train <dataset.json>\n"
            << "  local_intent_ai predict \"text\"\n"
            << std::endl;
    }

    std::string jsonEscape(
        const std::string& input)
    {
        std::string output;

        output.reserve(input.size());

        for (char c : input)
        {
            switch (c)
            {
                case '\\':
                    output += "\\\\";
                    break;

                case '"':
                    output += "\\\"";
                    break;

                case '\n':
                    output += "\\n";
                    break;

                case '\r':
                    output += "\\r";
                    break;

                case '\t':
                    output += "\\t";
                    break;

                default:
                    output += c;
                    break;
            }
        }

        return output;
    }

    void registerDefaultTools(
        ToolDispatcher& dispatcher)
    {
        dispatcher.registerTool(
            "open_browser",
            const ToolRequest&
            {
                return ToolResult{
                    true,
                    "Browser tool matched"
                };
            });

        dispatcher.registerTool(
            "open_editor",
            const ToolRequest&
            {
                return ToolResult{
                    true,
                    "Editor tool matched"
                };
            });

        dispatcher.registerTool(
            "play_music",
            const ToolRequest&
            {
                return ToolResult{
                    true,
                    "Music tool matched"
                };
            });

        dispatcher.registerTool(
            "search_web",
            const ToolRequest&
            {
                return ToolResult{
                    true,
                    "Search tool matched"
                };
            });
    }

} // namespace

int main(
    int argc,
    char* argv[])
{
    if (argc < 2)
    {
        printUsage();
        return EXIT_FAILURE;
    }

    const std::string command =
        argv[1];

    Database database;

    if (!database.open(DB_FILE))
    {
        std::cerr
            << "[Main] Unable to open database."
            << std::endl;
    }

    if (command == "train")
    {
        if (argc < 3)
        {
            printUsage();
            return EXIT_FAILURE;
        }

        const std::string datasetPath =
            argv[2];

        Trainer trainer;

        constexpr std::size_t epochs = 150;
        constexpr std::size_t batchSize = 8;

        const bool success =
            trainer.train(
                datasetPath,
                MODEL_DIR,
                epochs,
                batchSize);

        if (!success)
        {
            database.insertLog(
                "ERROR",
                "Training failed");

            return EXIT_FAILURE;
        }

        database.insertTrainingRun(
            datasetPath,
            epochs,
            trainer.intentCount(),
            trainer.vocabularySize(),
            trainer.intentCount());

        database.insertLog(
            "INFO",
            "Training completed");

        std::cout
            << "\nTraining completed successfully.\n"
            << std::endl;

        return EXIT_SUCCESS;
    }

    if (command == "predict")
    {
        if (argc < 3)
        {
            printUsage();
            return EXIT_FAILURE;
        }

        const std::string text =
            argv[2];

        Predictor predictor;

        if (!predictor.load(MODEL_DIR))
        {
            std::cerr
                << "[Main] Model loading failed."
                << std::endl;

            return EXIT_FAILURE;
        }

        PredictionResult prediction =
            predictor.predict(text);

        database.insertPrediction(
            text,
            prediction.intent,
            prediction.confidence);

        ToolDispatcher dispatcher;

        registerDefaultTools(dispatcher);

        ToolRequest request =
            dispatcher.createRequest(
                prediction);

        ToolResult toolResult =
            dispatcher.dispatch(
                request);

        std::ostringstream json;

        json
            << "{\n"
            << "  \"intent\":\""
            << jsonEscape(
                   prediction.intent)
            << "\",\n"
            << "  \"confidence\":"
            << prediction.confidence
            << ",\n"
            << "  \"tool_success\":"
            << (toolResult.success
                   ? "true"
                   : "false")
            << ",\n"
            << "  \"tool_message\":\""
            << jsonEscape(
                   toolResult.message)
            << "\"\n"
            << "}";

        std::cout
            << json.str()
            << std::endl;

        return EXIT_SUCCESS;
    }

    printUsage();

    return EXIT_FAILURE;
}
