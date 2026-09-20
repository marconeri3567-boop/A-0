#pragma once

#include "DatasetLoader.hpp"
#include "Vocabulary.hpp"
#include "NeuralModel.hpp"

#include <string>
#include <unordered_map>
#include <vector>

/**
 * @brief Coordinatore dell'intera pipeline di training.
 *
 * Pipeline:
 *
 * JSON Dataset
 *      |
 * DatasetLoader
 *      |
 * Vocabulary
 *      |
 * Bag Of Words
 *      |
 * NeuralModel
 *      |
 * Salvataggio Modello
 */
class Trainer
{
public:

    Trainer() = default;

    /**
     * @brief Addestra il modello da dataset JSON.
     *
     * Esegue:
     * - caricamento dataset
     * - costruzione vocabolario
     * - encoding intent
     * - training rete
     * - salvataggio modelli
     *
     * @param datasetPath file JSON input
     * @param outputDir directory models
     * @param epochs epoche training
     * @param batchSize dimensione batch
     *
     * @return true se completato
     */
    bool train(
        const std::string& datasetPath,
        const std::string& outputDir,
        std::size_t epochs = 100,
        std::size_t batchSize = 8
    );

    /**
     * @brief Accesso al modello addestrato.
     */
    [[nodiscard]]
    const NeuralModel& getModel() const noexcept;

    /**
     * @brief Accesso al vocabolario.
     */
    [[nodiscard]]
    const Vocabulary& getVocabulary() const noexcept;

    /**
     * @brief Numero intenti.
     */
    [[nodiscard]]
    std::size_t intentCount() const noexcept;

    /**
     * @brief Numero token vocabolario.
     */
    [[nodiscard]]
    std::size_t vocabularySize() const noexcept;

private:

    /**
     * @brief Costruisce la mappa intent -> indice.
     */
    void buildIntentMap(
        const std::vector<std::string>& intents
    );

    /**
     * @brief Restituisce l'indice di una label.
     */
    [[nodiscard]]
    int labelToIndex(
        const std::string& intent
    ) const;

    /**
     * @brief Converte dataset in input tiny-dnn.
     */
    bool buildTrainingData(
        const std::vector<TrainingSample>& samples,
        std::vector<tiny_dnn::vec_t>& inputs,
        std::vector<tiny_dnn::label_t>& labels
    );

private:

    DatasetLoader datasetLoader_;

    Vocabulary vocabulary_;

    NeuralModel neuralModel_;

    std::vector<std::string> intents_;

    std::unordered_map<
        std::string,
        std::size_t
    > intentMap_;
};
