#pragma once

#include "NeuralModel.hpp"

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

/**
 * @brief Descrive una richiesta di esecuzione.
 *
 * In futuro potrà essere popolata
 * da un Entity Extractor.
 */
struct ToolRequest
{
    std::string intent;
    std::string entity;
    float confidence = 0.0f;
};

/**
 * @brief Risultato esecuzione tool.
 */
struct ToolResult
{
    bool success = false;
    std::string message;
};

/**
 * @brief Tipo funzione handler.
 */
using ToolHandler =
    std::function<ToolResult(
        const ToolRequest&)>;
        
/**
 * @brief Dispatcher di intent -> azione.
 *
 * Esempio:
 *
 * open_browser
 *        |
 *        +--> handler
 *
 * open_editor
 *        |
 *        +--> handler
 */
class ToolDispatcher
{
public:

    ToolDispatcher() = default;

    /**
     * @brief Registra un handler.
     *
     * @param intent nome intento
     * @param handler callback esecuzione
     */
    void registerTool(
        const std::string& intent,
        ToolHandler handler
    );

    /**
     * @brief Verifica registrazione.
     */
    [[nodiscard]]
    bool hasTool(
        const std::string& intent
    ) const;

    /**
     * @brief Esegue richiesta.
     */
    [[nodiscard]]
    ToolResult dispatch(
        const ToolRequest& request
    ) const;

    /**
     * @brief Converte risultato ML
     * in richiesta tool.
     */
    [[nodiscard]]
    ToolRequest createRequest(
        const PredictionResult& prediction
    ) const;

    /**
     * @brief Numero tool registrati.
     */
    [[nodiscard]]
    std::size_t size() const noexcept;

    /**
     * @brief Elenco intent gestiti.
     */
    [[nodiscard]]
    std::vector<std::string>
    availableTools() const;

private:

    std::unordered_map<
        std::string,
        ToolHandler
    > tools_;
};
