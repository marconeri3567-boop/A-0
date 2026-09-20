#pragma once

#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>

#include <tiny_dnn/tiny_dnn.h>

/**
 * @brief Risultato di inferenza.
 */
struct PredictionResult
{
    std::string intent;
    float confidence = 0.0f;
};

/**
 * @brief Wrapper della rete neurale Tiny-DNN.
 *
 * Responsabilità:
 * - costruzione rete
 * - training
 * - inferenza
 * - serializzazione
 * - gestione intenti
 */
class NeuralModel
{
public:

    NeuralModel();

    ~NeuralModel() = default;

    /**
     * @brief Costruisce la rete neurale.
     *
     * @param inputSize dimensione vettore input
     * @param outputSize numero classi
     */
    void initialize(
        std::size_t inputSize,
        std::size_t outputSize
    );

    /**
     * @brief Addestra il modello.
     */
    bool train(
        const std::vector<tiny_dnn::vec_t>& inputs,
        const std::vector<tiny_dnn::label_t>& labels,
        std::size_t epochs = 100,
        std::size_t batchSize = 8
    );

    /**
     * @brief Classifica un vettore BoW.
     */
    [[nodiscard]]
    PredictionResult predict(
        const std::vector<float>& inputVector
    ) const;

    /**
     * @brief Salvataggio modello.
     */
    bool saveModel(
        const std::string& modelPath
    ) const;

    /**
     * @brief Caricamento modello.
     */
    bool loadModel(
        const std::string& modelPath
    );

    /**
     * @brief Salvataggio metadata classi.
     */
    bool saveLabels(
        const std::string& labelPath
    ) const;

    /**
     * @brief Caricamento metadata classi.
     */
    bool loadLabels(
        const std::string& labelPath
    );

    /**
     * @brief Imposta gli intenti disponibili.
     */
    void setIntents(
        const std::vector<std::string>& intents
    );

    /**
     * @brief Restituisce la lista intenti.
     */
    [[nodiscard]]
    const std::vector<std::string>&
    getIntents() const noexcept;

    /**
     * @brief Numero classi.
     */
    [[nodiscard]]
    std::size_t classCount() const noexcept;

    /**
     * @brief Dimensione input.
     */
    [[nodiscard]]
    std::size_t inputSize() const noexcept;

    /**
     * @brief Dimensione output.
     */
    [[nodiscard]]
    std::size_t outputSize() const noexcept;

    /**
     * @brief Modello inizializzato.
     */
    [[nodiscard]]
    bool isInitialized() const noexcept;

private:

    /**
     * @brief Intent -> indice.
     */
    [[nodiscard]]
    int intentToIndex(
        const std::string& intent
    ) const;

    /**
     * @brief Indice -> intent.
     */
    [[nodiscard]]
    std::string indexToIntent(
        std::size_t index
    ) const;

    /**
     * @brief Probabilità massima.
     */
    [[nodiscard]]
    float maxProbability(
        const tiny_dnn::vec_t& output
    ) const;

private:

    tiny_dnn::network<
        tiny_dnn::sequential
    > network_;

    std::vector<std::string> intents_;

    std::unordered_map<
        std::string,
        std::size_t
    > intentToIndexMap_;

    std::size_t inputSize_ = 0;

    std::size_t outputSize_ = 0;

    bool initialized_ = false;
};

