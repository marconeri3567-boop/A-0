#include "Trainer.hpp"

#include <algorithm>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

bool Trainer::train(
    const std::string& datasetPath,
    const std::string& outputDir,
    std::size_t epochs,
    std::size_t batchSize)
{
    std::cout
        << "[Trainer] Loading dataset..."
        << std::endl;

    if (!datasetLoader_.load(datasetPath))
    {
        std::cerr
            << "[Trainer] Dataset loading failed."
            << std::endl;

        return false;
    }

    const auto& samples =
        datasetLoader_.getSamples();

    if (samples.empty())
    {
        std::cerr
            << "[Trainer] Empty dataset."
            << std::endl;

        return false;
    }

    std::cout
        << "[Trainer] Building vocabulary..."
        << std::endl;

    vocabulary_.build(samples);

    if (vocabulary_.empty())
    {
        std::cerr
            << "[Trainer] Empty vocabulary."
            << std::endl;

        return false;
    }

    intents_ =
        datasetLoader_.getUniqueIntents();

    if (intents_.empty())
    {
        std::cerr
            << "[Trainer] No intents detected."
            << std::endl;

        return false;
    }

    std::sort(
        intents_.begin(),
        intents_.end());

    buildIntentMap(intents_);

    neuralModel_.setIntents(intents_);

    std::vector<tiny_dnn::vec_t> inputs;
    std::vector<tiny_dnn::label_t> labels;

    std::cout
        << "[Trainer] Building training data..."
        << std::endl;

    if (!buildTrainingData(
            samples,
            inputs,
            labels))
    {
        std::cerr
            << "[Trainer] Training data generation failed."
            << std::endl;

        return false;
    }

    std::cout
        << "[Trainer] Vocabulary size: "
        << vocabulary_.size()
        << std::endl;

    std::cout
        << "[Trainer] Intent count: "
        << intents_.size()
        << std::endl;

    neuralModel_.initialize(
        vocabulary_.size(),
        intents_.size());

    std::cout
        << "[Trainer] Training neural network..."
        << std::endl;

    if (!neuralModel_.train(
            inputs,
            labels,
            epochs,
            batchSize))
    {
        std::cerr
            << "[Trainer] Training failed."
            << std::endl;

        return false;
    }

    try
    {
        fs::create_directories(outputDir);
    }
    catch (...)
    {
        std::cerr
            << "[Trainer] Unable to create output directory."
            << std::endl;

        return false;
    }

    const std::string modelFile =
        outputDir + "/model.dat";

    const std::string labelsFile =
        outputDir + "/labels.json";

    const std::string vocabularyFile =
        outputDir + "/vocabulary.json";

    std::cout
        << "[Trainer] Saving artifacts..."
        << std::endl;

    if (!neuralModel_.saveModel(modelFile))
    {
        std::cerr
            << "[Trainer] Model save failed."
            << std::endl;

        return false;
    }

    if (!neuralModel_.saveLabels(labelsFile))
    {
        std::cerr
            << "[Trainer] Label save failed."
            << std::endl;

        return false;
    }

    if (!vocabulary_.save(vocabularyFile))
    {
        std::cerr
            << "[Trainer] Vocabulary save failed."
            << std::endl;

        return false;
    }

    std::cout
        << "[Trainer] Training completed."
        << std::endl;

    std::cout
        << "[Trainer] Model saved in: "
        << outputDir
        << std::endl;

    return true;
}

const NeuralModel&
Trainer::getModel() const noexcept
{
    return neuralModel_;
}

const Vocabulary&
Trainer::getVocabulary() const noexcept
{
    return vocabulary_;
}

std::size_t Trainer::intentCount() const noexcept
{
    return intents_.size();
}

std::size_t Trainer::vocabularySize() const noexcept
{
    return vocabulary_.size();
}

void Trainer::buildIntentMap(
    const std::vector<std::string>& intents)
{
    intentMap_.clear();

    intentMap_.reserve(intents.size());

    for (std::size_t i = 0;
         i < intents.size();
         ++i)
    {
        intentMap_.emplace(
            intents[i],
            i);
    }
}

int Trainer::labelToIndex(
    const std::string& intent) const
{
    const auto it =
        intentMap_.find(intent);

    if (it == intentMap_.end())
    {
        return -1;
    }

    return static_cast<int>(it->second);
}

bool Trainer::buildTrainingData(
    const std::vector<TrainingSample>& samples,
    std::vector<tiny_dnn::vec_t>& inputs,
    std::vector<tiny_dnn::label_t>& labels)
{
    inputs.clear();
    labels.clear();

    inputs.reserve(samples.size());
    labels.reserve(samples.size());

    for (const auto& sample : samples)
    {
        const int labelIndex =
            labelToIndex(sample.intent);

        if (labelIndex < 0)
        {
            continue;
        }

        const auto bow =
            vocabulary_.textToVector(
                sample.input);

        tiny_dnn::vec_t input(
            bow.begin(),
            bow.end());

        inputs.emplace_back(
            std::move(input));

        labels.emplace_back(
            static_cast<tiny_dnn::label_t>(
                labelIndex));
    }

    return
        !inputs.empty() &&
        inputs.size() == labels.size();
}
