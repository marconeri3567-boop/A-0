#include "Database.hpp"

#include <iostream>
#include <sstream>

Database::~Database() { close(); }

bool Database::open(const std::string& databaseFile)
{
    close();
    if (sqlite3_open(databaseFile.c_str(), &database_) != SQLITE_OK) {
        std::cerr << "[Database] Open error: " << (database_ ? sqlite3_errmsg(database_) : "unknown") << '\n';
        close();
        return false;
    }
    execute("PRAGMA journal_mode=WAL;");
    execute("PRAGMA synchronous=NORMAL;");
    execute("PRAGMA foreign_keys=ON;");
    return initialize();
}

void Database::close() { if (database_) { sqlite3_close(database_); database_ = nullptr; } }
bool Database::isOpen() const noexcept { return database_ != nullptr; }

bool Database::initialize()
{
    if (!database_) return false;
    return execute("CREATE TABLE IF NOT EXISTS predictions (id INTEGER PRIMARY KEY AUTOINCREMENT, input_text TEXT NOT NULL, predicted_intent TEXT NOT NULL, confidence REAL NOT NULL, created_at DATETIME DEFAULT CURRENT_TIMESTAMP);") &&
           execute("CREATE TABLE IF NOT EXISTS training_runs (id INTEGER PRIMARY KEY AUTOINCREMENT, dataset_path TEXT NOT NULL, epochs INTEGER NOT NULL, samples INTEGER NOT NULL, vocabulary_size INTEGER NOT NULL, intents INTEGER NOT NULL, created_at DATETIME DEFAULT CURRENT_TIMESTAMP);") &&
           execute("CREATE TABLE IF NOT EXISTS logs (id INTEGER PRIMARY KEY AUTOINCREMENT, level TEXT NOT NULL, message TEXT NOT NULL, created_at DATETIME DEFAULT CURRENT_TIMESTAMP);");
}

bool Database::insertPrediction(const std::string&, const std::string&, float) { return true; }
bool Database::insertTrainingRun(const std::string&, std::size_t, std::size_t, std::size_t, std::size_t) { return true; }
bool Database::insertLog(const std::string&, const std::string&) { return true; }
std::vector<PredictionRecord> Database::getPredictions(std::size_t) const { return {}; }
std::vector<TrainingRecord> Database::getTrainingRuns(std::size_t) const { return {}; }
std::vector<LogRecord> Database::getLogs(std::size_t) const { return {}; }
bool Database::clearPredictions() { return database_ && execute("DELETE FROM predictions;"); }
std::size_t Database::predictionCount() const { return 0; }
bool Database::execute(const std::string& sql) const
{
    if (!database_) return false;
    char* error = nullptr;
    const int rc = sqlite3_exec(database_, sql.c_str(), nullptr, nullptr, &error);
    if (rc != SQLITE_OK) { if (error) sqlite3_free(error); return false; }
    return true;
}
