#pragma once

#include "DatasetLoader.hpp"

#include <cstddef>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

/**
 * @brief Gestione del vocabolario e conversione testo -> vettore.
 *
 * Implementa:
 * - normalizzazione
 * - tokenizzazione
 * - costruzione vocabolario
 * - Bag Of Words
 * - serializzazione JSON
 */
class Vocabulary
{
public:

    Vocabulary() = default;

    /**
     * @brief Costruisce il vocabolario dai campioni.
     *
     * @param samples dataset di training
     */
    void build(
        const std::vector<TrainingSample>& samples
    );

    /**
     * @brief Converte testo in token.
     *
     * @param text frase da analizzare
     * @return lista token
     */
    [[nodiscard]]
    std::vector<std::string> tokenize(
        const std::string& text
    ) const;

    /**
     * @brief Normalizza il testo.
     *
     * Operazioni:
     * - lowercase
     * - rimozione punteggiatura
     * - compressione spazi
     */
    [[nodiscard]]
    std::string normalize(
        const std::string& text
    ) const;

    /**
     * @brief Converte frase in vettore Bag Of Words.
     *
     * Dimensione:
     * size() elementi.
     */
    [[nodiscard]]
    std::vector<float> textToVector(
        const std::string& text
    ) const;

    /**
     * @brief Verifica presenza token.
     */
    [[nodiscard]]
    bool contains(
        const std::string& token
    ) const;

    /**
     * @brief Restituisce indice token.
     *
     * @return -1 se assente
     */
    [[nodiscard]]
    int getIndex(
        const std::string& token
    ) const;

    /**
     * @brief Restituisce token dato indice.
     *
     * @return stringa vuota se indice non valido
     */
    [[nodiscard]]
    std::string getToken(
        std::size_t index
    ) const;

    /**
     * @brief Numero token del vocabolario.
     */
    [[nodiscard]]
    std::size_t size() const noexcept;

    /**
     * @brief Vocabolario vuoto.
     */
    [[nodiscard]]
    bool empty() const noexcept;

    /**
     * @brief Salvataggio vocabolario.
     *
     * File JSON.
     */
    bool save(
        const std::string& filePath
    ) const;

    /**
     * @brief Caricamento vocabolario.
     */
    bool load(
        const std::string& filePath
    );

    /**
     * @brief Accesso completo ai token.
     */
    [[nodiscard]]
    const std::vector<std::string>&
    getTokens() const noexcept;

private:

    /**
     * @brief Inserisce token univoco.
     */
    void addToken(
        const std::string& token
    );

private:

    /**
     * Token ordinati per indice.
     */
    std::vector<std::string> tokens_;

    /**
     * Token -> indice.
     */
    std::unordered_map<std::string, std::size_t>
        tokenToIndex_;
};
