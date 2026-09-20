#pragma once

#include "Vocabulary.hpp"
#include "NeuralModel.hpp"

#include <string>

/**
 * @brief Motore di inferenza realtime.
 *
 * Carica:
 * - modello
 * - vocabolario
 * - labels
 *
 * e fornisce API ad alto livello
 * per la classificazione degli intenti.
 */
class Predictor
{
public:

    Predictor() = default;

    /**
     * @brief Carica tutti gli artefatti del modello.
     *
     * Directory attesa:
     *
     * models/
     *   model.dat
     *   labels.json
     *   vocabulary.json
     *
     * @param modelDir directory modelli
     * @return true se caricamento riuscito
     */
    bool load(
        const std::string& modelDir
    );

    /**
     * @brief Predice l'intento di una frase.
     *
     * Esempio:
     *
     * input:
     * "apri firefox"
     *
     * output:
     * {
     *   "intent":"open_browser",
     *   "confidence":0.95
     * }
     */
    [[nodiscard]]
    PredictionResult predict(
        const std::string& text
    ) const;

    /**
     * @brief Verifica se il predictor
     * è pronto all'uso.
     */
    [[nodiscard]]
    bool isReady() const noexcept;

    /**
     * @brief Accesso al vocabolario.
     */
    [[nodiscard]]
    const Vocabulary&
    getVocabulary() const noexcept;

    /**
     * @brief Accesso al modello.
     */
    [[nodiscard]]
    const NeuralModel&
    getModel() const noexcept;

private:

    Vocabulary vocabulary_;

    NeuralModel neuralModel_;

    bool loaded_ = false;
};
