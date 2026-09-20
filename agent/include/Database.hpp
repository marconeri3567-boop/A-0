#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include <sqlite3.h>

/**
 * @brief Record storico inferenza.
 */
struct PredictionRecord
{
    std::int64_t id = 0;

    std::string inputText;

    std::string predictedIntent;

    float confidence = 0.0f;

    std::string createdAt;
};

/**
 * @brief Metriche training.
 */
struct TrainingRecord
{
    std::int64_t id = 0;

    std::string datasetPath;

    std::size_t epochs = 0;

    std::size_t samples = 0;

    std::size_t vocabularySize = 0;

    std::size_t intents = 0;

    std::string createdAt;
};

/**
 * @brief Record generico di log.
 */
struct LogRecord
{
    std::int64_t id = 0;

    std::string level;

    std::string message;

    std::string createdAt;
};

/**
 * @brief Wrapper SQLite3.
 *
 * Responsabilità:
 * - apertura database
 * - creazione schema
 * - inserimento storico
 * - recupero statistiche
 * - log applicativi
 */
class Database
{
public:

    Database() = default;

    ~Database();

    /**
     * @brief Apre o crea il database.
     */
    bool open(
        const std::string& databaseFile
    );

    /**
     * @brief Chiude il database.
     */
    void close();

    /**
     * @brief Verifica connessione attiva.
     */
    [[nodiscard]]
    bool isOpen() const noexcept;

    /**
     * @brief Crea lo schema iniziale.
     */
    bool initialize();

    /**
     * @brief Inserisce inferenza.
     */
    bool insertPrediction(
        const std::string& inputText,
        const std::string& predictedIntent,
        float confidence
    );

    /**
     * @brief Inserisce metrica training.
     */
    bool insertTrainingRun(
        const std::string& datasetPath,
        std::size_t epochs,
        std::size_t samples,
        std::size_t vocabularySize,
        std::size_t intents
    );

    /**
     * @brief Inserisce log.
     */
    bool insertLog(
        const std::string& level,
        const std::string& message
    );

    /**
     * @brief Restituisce ultime predizioni.
     */
    [[nodiscard]]
    std::vector<PredictionRecord>
    getPredictions(
        std::size_t limit = 100
    ) const;

    /**
     * @brief Restituisce ultimi training.
     */
    [[nodiscard]]
    std::vector<TrainingRecord>
    getTrainingRuns(
        std::size_t limit = 50
    ) const;

    /**
     * @brief Restituisce ultimi log.
     */
    [[nodiscard]]
    std::vector<LogRecord>
    getLogs(
        std::size_t limit = 100
    ) const;

    /**
     * @brief Elimina storico inferenze.
     */
    bool clearPredictions();

    /**
     * @brief Conteggio inferenze.
     */
    [[nodiscard]]
    std::size_t predictionCount() const;

private:

    /**
     * @brief Esegue statement SQL.
     */
    bool execute(
        const std::string& sql
    ) const;

private:

    sqlite3* database_ = nullptr;
};
