#pragma once

#include "DatasetConverter.hpp"
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
 *      DatasetConverter
 *      |
 *      DatasetLoader
 *      |
 *      Vocabulary
 *      |
 *      Bag Of Words
 *      |
 *      NeuralModel
 *      |
 *      Salvataggio Modello
 */
class Trainer
{
public:
    Trainer() = default;

    bool train(
        const std::string& datasetPath,
        const std::string& outputDir,
        std::size_t epochs = 100,
        std::size_t batchSize = 8
    );

    [[nodiscard]] const NeuralModel& getModel() const noexcept;
    [[nodiscard]] const Vocabulary& getVocabulary() const noexcept;
    [[nodiscard]] std::size_t intentCount() const noexcept;
    [[nodiscard]] std::size_t vocabularySize() const noexcept;

private:
    void buildIntentMap(const std::vector<std::string>& intents);
    [[nodiscard]] int labelToIndex(const std::string& intent) const;

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
    std::unordered_map<std::string, std::size_t> intentMap_;
};
