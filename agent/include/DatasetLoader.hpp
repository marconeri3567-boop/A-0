#pragma once

#include <string>
#include <vector>
#include <unordered_set>

/**
 * @brief Singolo campione di addestramento.
 *
 * Esempio:
 *
 * {
 *    "input": "apri firefox",
 *    "intent": "open_browser"
 * }
 */
struct TrainingSample
{
    std::string input;
    std::string intent;
};

/**
 * @brief Caricatore dataset JSON.
 *
 * Responsabilità:
 * - apertura file JSON
 * - validazione struttura
 * - caricamento campioni
 * - estrazione intent unici
 */
class DatasetLoader
{
public:

    DatasetLoader() = default;

    /**
     * @brief Carica dataset da file JSON.
     *
     * @param jsonPath percorso file dataset
     * @return true se caricamento riuscito
     * @return false in caso di errore
     */
    bool load(const std::string& jsonPath);

    /**
     * @brief Restituisce tutti i campioni caricati.
     */
    [[nodiscard]]
    const std::vector<TrainingSample>& getSamples() const noexcept;

    /**
     * @brief Numero di campioni.
     */
    [[nodiscard]]
    std::size_t size() const noexcept;

    /**
     * @brief Dataset vuoto.
     */
    [[nodiscard]]
    bool empty() const noexcept;

    /**
     * @brief Intent unici presenti nel dataset.
     */
    [[nodiscard]]
    std::vector<std::string> getUniqueIntents() const;

    /**
     * @brief Verifica se il dataset è valido.
     */
    [[nodiscard]]
    bool isValid() const noexcept;

private:

    /**
     * @brief Verifica validità singolo campione.
     */
    bool validateSample(
        const std::string& input,
        const std::string& intent
    ) const noexcept;

private:

    std::vector<TrainingSample> samples_;

    bool valid_ = false;
};
