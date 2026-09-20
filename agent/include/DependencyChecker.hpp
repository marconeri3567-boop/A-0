#pragma once

#include <string>
#include <vector>

struct DependencyStatus
{
    std::string name;
    bool available = false;
    std::string version;
    std::string suggestion;
};

struct DependencyReport
{
    bool ready = false;
    std::vector<DependencyStatus> dependencies;
    std::string platform;
    std::string packageManager;
};

/**
 * @brief Verifica gli strumenti necessari per compilare/eseguire LocalIntentAI.
 *
 * Il controllo non installa pacchetti automaticamente: restituisce comandi
 * riproducibili e sicuri per il package manager rilevato. Le librerie C++
 * gestite da CMake FetchContent (Eigen, nlohmann/json e tiny-dnn) vengono
 * indicate come dipendenze risolte durante la configurazione.
 */
class DependencyChecker
{
public:
    [[nodiscard]] DependencyReport check() const;
    [[nodiscard]] std::string format(const DependencyReport& report) const;

private:
    [[nodiscard]] bool commandAvailable(const std::string& command) const;
    [[nodiscard]] std::string commandVersion(const std::string& command) const;
    [[nodiscard]] std::string detectPlatform() const;
    [[nodiscard]] std::string detectPackageManager() const;
    [[nodiscard]] std::string installSuggestion(const std::string& packageManager) const;
};
