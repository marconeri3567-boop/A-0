#pragma once

#include <string>

/**
 * @brief Risultato della preparazione di un dataset.
 */
struct DatasetPreparation
{
    std::string path;
    bool converted = false;
};

/**
 * @brief Converte i formati JSON supportati nel formato interno del trainer.
 *
 * Formato interno:
 * {
 *     "input": "...",
 *     "intent": "..."
 * }
 *
 * Formato legacy supportato:
 * {
 *     "request": "...",
 *     "answer": "..."
 * }
 *
 * I dataset già nel formato interno vengono restituiti senza creare una
 * copia. I dataset legacy o misti vengono normalizzati in outputDir.
 */
class DatasetConverter
{
public:
    /**
     * @brief Prepara un dataset per DatasetLoader.
     *
     * @param inputPath dataset JSON originale
     * @param outputDir directory per l'eventuale dataset normalizzato
     * @param preparation percorso da usare nel loader e stato conversione
     * @return true se il JSON è leggibile e ha un formato supportato
     */
    bool prepare(
        const std::string& inputPath,
        const std::string& outputDir,
        DatasetPreparation& preparation
    ) const;
};
