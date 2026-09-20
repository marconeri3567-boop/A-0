#include "Database.hpp"

#include <iostream>
#include <sstream>

Database::~Database()
{
    close();
}

bool Database::open(
    const std::string& databaseFile)
{
    close();

    const int rc =
        sqlite3_open(
            databaseFile.c_str(),
            &database_);

    if (rc != SQLITE_OK)
    {
        std::cerr
            << "[Database] Open error: "
            << sqlite3_errmsg(database_)
            << std::endl;

        close();

        return false;
    }

    execute("PRAGMA journal_mode=WAL;");
    execute("PRAGMA synchronous=NORMAL;");
    execute("PRAGMA foreign_keys=ON;");

    return initialize();
}

void Database::close()
{
    if (database_ != nullptr)
    {
        sqlite3_close(database_);
        database_ = nullptr;
    }
}

bool Database::isOpen() const noexcept
{
    return database_ != nullptr;
}

bool Database::initialize()
{
    const char* predictionsTable =
        R"(
        CREATE TABLE IF NOT EXISTS predictions
        (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            input_text TEXT NOT NULL,
            predicted_intent TEXT NOT NULL,
            confidence REAL NOT NULL,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP
        );
        )";

    const char* trainingTable =
        R"(
        CREATE TABLE IF NOT EXISTS training_runs
        (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            dataset_path TEXT NOT NULL,
            epochs INTEGER NOT NULL,
            samples INTEGER NOT NULL,
            vocabulary_size INTEGER NOT NULL,
            intents INTEGER NOT NULL,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP
        );
        )";

    const char* logsTable =
        R"(
        CREATE TABLE IF NOT EXISTS logs
        (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            level TEXT NOT NULL,
            message TEXT NOT NULL,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP
        );
        )";

    return
        execute(predictionsTable) &&
        execute(trainingTable) &&
        execute(logsTable);
}

bool Database::insertPrediction(
    const std::string& inputText,
    const std::string& predictedIntent,
    float confidence)
{
    constexpr const char* sql =
        R"(
        INSERT INTO predictions
        (
            input_text,
            predicted_intent,
            confidence
        )
        VALUES (?, ?, ?);
        )";

    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(
            database_,
            sql,
            -1,
            &stmt,
            nullptr) != SQLITE_OK)
    {
        return false;
    }

    sqlite3_bind_text(
        stmt,
        1,
        inputText.c_str(),
        -1,
        SQLITE_TRANSIENT);

    sqlite3_bind_text(
        stmt,
        2,
        predictedIntent.c_str(),
        -1,
        SQLITE_TRANSIENT);

    sqlite3_bind_double(
        stmt,
        3,
        static_cast<double>(confidence));

    const bool success =
        sqlite3_step(stmt) == SQLITE_DONE;

    sqlite3_finalize(stmt);

    return success;
}

bool Database::insertTrainingRun(
    const std::string& datasetPath,
    std::size_t epochs,
    std::size_t samples,
    std::size_t vocabularySize,
    std::size_t intents)
{
    constexpr const char* sql =
        R"(
        INSERT INTO training_runs
        (
            dataset_path,
            epochs,
            samples,
            vocabulary_size,
            intents
        )
        VALUES (?, ?, ?, ?, ?);
        )";

    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(
            database_,
            sql,
            -1,
            &stmt,
            nullptr) != SQLITE_OK)
    {
        return false;
    }

    sqlite3_bind_text(
        stmt,
        1,
        datasetPath.c_str(),
        -1,
        SQLITE_TRANSIENT);

    sqlite3_bind_int64(
        stmt,
        2,
        static_cast<sqlite3_int64>(epochs));

    sqlite3_bind_int64(
        stmt,
        3,
        static_cast<sqlite3_int64>(samples));

    sqlite3_bind_int64(
        stmt,
        4,
        static_cast<sqlite3_int64>(vocabularySize));

    sqlite3_bind_int64(
        stmt,
        5,
        static_cast<sqlite3_int64>(intents));

    const bool success =
        sqlite3_step(stmt) == SQLITE_DONE;

    sqlite3_finalize(stmt);

    return success;
}

bool Database::insertLog(
    const std::string& level,
    const std::string& message)
{
    constexpr const char* sql =
        R"(
        INSERT INTO logs
        (
            level,
            message
        )
        VALUES (?, ?);
        )";

    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(
            database_,
            sql,
            -1,
            &stmt,
            nullptr) != SQLITE_OK)
    {
        return false;
    }

    sqlite3_bind_text(
        stmt,
        1,
        level.c_str(),
        -1,
        SQLITE_TRANSIENT);

    sqlite3_bind_text(
        stmt,
        2,
        message.c_str(),
        -1,
        SQLITE_TRANSIENT);

    const bool success =
        sqlite3_step(stmt) == SQLITE_DONE;

    sqlite3_finalize(stmt);

    return success;
}

std::vector<PredictionRecord>
Database::getPredictions(
    std::size_t limit) const
{
    std::vector<PredictionRecord> results;

    std::stringstream query;

    query
        << "SELECT "
        << "id,"
        << "input_text,"
        << "predicted_intent,"
        << "confidence,"
        << "created_at "
        << "FROM predictions "
        << "ORDER BY id DESC "
        << "LIMIT "
        << limit;

    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(
            database_,
            query.str().c_str(),
            -1,
            &stmt,
            nullptr) != SQLITE_OK)
    {
        return results;
    }

    while (sqlite3_step(stmt)
           == SQLITE_ROW)
    {
        PredictionRecord record;

        record.id =
            sqlite3_column_int64(stmt, 0);

        record.inputText =
            reinterpret_cast<
                const char*>(
                    sqlite3_column_text(
                        stmt, 1));

        record.predictedIntent =
            reinterpret_cast<
                const char*>(
                    sqlite3_column_text(
                        stmt, 2));

        record.confidence =
            static_cast<float>(
                sqlite3_column_double(
                    stmt, 3));

        record.createdAt =
            reinterpret_cast<
                const char*>(
                    sqlite3_column_text(
                        stmt, 4));

        results.push_back(
            std::move(record));
    }

    sqlite3_finalize(stmt);

    return results;
}

std::vector<TrainingRecord>
Database::getTrainingRuns(
    std::size_t limit) const
{
    std::vector<TrainingRecord> results;

    std::stringstream query;

    query
        << "SELECT "
        << "id,"
        << "dataset_path,"
        << "epochs,"
        << "samples,"
        << "vocabulary_size,"
        << "intents,"
        << "created_at "
        << "FROM training_runs "
        << "ORDER BY id DESC "
        << "LIMIT "
        << limit;

    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(
            database_,
            query.str().c_str(),
            -1,
            &stmt,
            nullptr) != SQLITE_OK)
    {
        return results;
    }

    while (sqlite3_step(stmt)
           == SQLITE_ROW)
    {
        TrainingRecord record;

        record.id =
            sqlite3_column_int64(stmt, 0);

        record.datasetPath =
            reinterpret_cast<
                const char*>(
                    sqlite3_column_text(
                        stmt, 1));

        record.epochs =
            static_cast<std::size_t>(
                sqlite3_column_int64(
                    stmt, 2));

        record.samples =
            static_cast<std::size_t>(
                sqlite3_column_int64(
                    stmt, 3));

        record.vocabularySize =
            static_cast<std::size_t>(
                sqlite3_column_int64(
                    stmt, 4));

        record.intents =
            static_cast<std::size_t>(
                sqlite3_column_int64(
                    stmt, 5));

        record.createdAt =
            reinterpret_cast<
                const char*>(
                    sqlite3_column_text(
                        stmt, 6));

        results.push_back(
            std::move(record));
    }

    sqlite3_finalize(stmt);

    return results;
}

std::vector<LogRecord>
Database::getLogs(
    std::size_t limit) const
{
    std::vector<LogRecord> results;

    std::stringstream query;

    query
        << "SELECT "
        << "id,"
        << "level,"
        << "message,"
        << "created_at "
        << "FROM logs "
        << "ORDER BY id DESC "
        << "LIMIT "
        << limit;

    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(
            database_,
            query.str().c_str(),
            -1,
            &stmt,
            nullptr) != SQLITE_OK)
    {
        return results;
    }

    while (sqlite3_step(stmt)
           == SQLITE_ROW)
    {
        LogRecord record;

        record.id =
            sqlite3_column_int64(stmt, 0);

        record.level =
            reinterpret_cast<
                const char*>(
                    sqlite3_column_text(
                        stmt, 1));

        record.message =
            reinterpret_cast<
                const char*>(
                    sqlite3_column_text(
                        stmt, 2));

        record.createdAt =
            reinterpret_cast<
                const char*>(
                    sqlite3_column_text(
                        stmt, 3));

        results.push_back(
            std::move(record));
    }

    sqlite3_finalize(stmt);

    return results;
}

bool Database::clearPredictions()
{
    return execute(
        "DELETE FROM predictions;");
}

std::size_t Database::predictionCount() const
{
    constexpr const char* sql =
        "SELECT COUNT(*) FROM predictions;";

    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(
            database_,
            sql,
            -1,
            &stmt,
            nullptr) != SQLITE_OK)
    {
        return 0;
    }

    std::size_t count = 0;

    if (sqlite3_step(stmt)
        == SQLITE_ROW)
    {
        count =
            static_cast<std::size_t>(
                sqlite3_column_int64(
                    stmt,
                    0));
    }

    sqlite3_finalize(stmt);

    return count;
}

bool Database::execute(
    const std::string& sql) const
{
    char* error = nullptr;

    const int rc =
        sqlite3_exec(
            database_,
            sql.c_str(),
            nullptr,
            nullptr,
            &error);

    if (rc != SQLITE_OK)
    {
        if (error != nullptr)
        {
            std::cerr
                << "[Database] SQL Error: "
                << error
                << std::endl;

            sqlite3_free(error);
        }

        return false;
    }

    return true;
}
