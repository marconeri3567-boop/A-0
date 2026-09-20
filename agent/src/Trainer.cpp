#include "Trainer.hpp"

#include <algorithm>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

bool Trainer::train(const std::string& datasetPath, const std::string& outputDir,
                    std::size_t epochs, std::size_t batchSize)
{
    DatasetConverter converter;
    DatasetPreparation preparation;
    if (!converter.prepare(datasetPath, outputDir, preparation)) return false;

    if (!datasetLoader_.load(preparation.path)) return false;
    const auto& samples = datasetLoader_.getSamples();
    vocabulary_.build(samples);
    if (vocabulary_.empty()) return false;

    intents_ = datasetLoader_.getUniqueIntents();
    if (intents_.empty()) return false;
    std::sort(intents_.begin(), intents_.end());
    buildIntentMap(intents_);
    neuralModel_.setIntents(intents_);

    std::vector<tiny_dnn::vec_t> inputs;
    std::vector<tiny_dnn::label_t> labels;
    if (!buildTrainingData(samples, inputs, labels)) return false;
    neuralModel_.initialize(vocabulary_.size(), intents_.size());
    if (!neuralModel_.train(inputs, labels, epochs, batchSize)) return false;

    try { fs::create_directories(outputDir); }
    catch (const fs::filesystem_error& ex) {
        std::cerr << "[Trainer] Unable to create output directory: " << ex.what() << '\n';
        return false;
    }
    return neuralModel_.saveModel(outputDir + "/model.dat") &&
           neuralModel_.saveLabels(outputDir + "/labels.json") &&
           vocabulary_.save(outputDir + "/vocabulary.json");
}

const NeuralModel& Trainer::getModel() const noexcept { return neuralModel_; }
const Vocabulary& Trainer::getVocabulary() const noexcept { return vocabulary_; }
std::size_t Trainer::intentCount() const noexcept { return intents_.size(); }
std::size_t Trainer::vocabularySize() const noexcept { return vocabulary_.size(); }

void Trainer::buildIntentMap(const std::vector<std::string>& intents)
{
    intentMap_.clear();
    for (std::size_t i = 0; i < intents.size(); ++i) intentMap_.emplace(intents[i], i);
}

int Trainer::labelToIndex(const std::string& intent) const
{
    const auto it = intentMap_.find(intent);
    return it == intentMap_.end() ? -1 : static_cast<int>(it->second);
}

bool Trainer::buildTrainingData(const std::vector<TrainingSample>& samples,
                                std::vector<tiny_dnn::vec_t>& inputs,
                                std::vector<tiny_dnn::label_t>& labels)
{
    inputs.clear(); labels.clear();
    for (const auto& sample : samples) {
        const int label = labelToIndex(sample.intent);
        if (label < 0) continue;
        const auto bow = vocabulary_.textToVector(sample.input);
        inputs.emplace_back(bow.begin(), bow.end());
        labels.push_back(static_cast<tiny_dnn::label_t>(label));
    }
    return !inputs.empty() && inputs.size() == labels.size();
}
