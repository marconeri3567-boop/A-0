#include "Predictor.hpp"

#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

bool Predictor::load(
    const std::string& modelDir)
{
    loaded_ = false;

    const std::string vocabularyFile =
        modelDir + "/vocabulary.json";

    const std::string labelsFile =
        modelDir + "/labels.json";

    const std::string modelFile =
        modelDir + "/model.dat";

    if (!fs::exists(vocabularyFile))
    {
        std::cerr
            << "[Predictor] Missing vocabulary: "
            << vocabularyFile
            << std::endl;

        return false;
    }

    if (!fs::exists(labelsFile))
    {
        std::cerr
            << "[Predictor] Missing labels: "
            << labelsFile
            << std::endl;

        return false;
    }

    if (!fs::exists(modelFile))
    {
        std::cerr
            << "[Predictor] Missing model: "
            << modelFile
            << std::endl;

        return false;
    }

    std::cout
        << "[Predictor] Loading vocabulary..."
        << std::endl;

    if (!vocabulary_.load(vocabularyFile))
    {
        std::cerr
            << "[Predictor] Failed loading vocabulary."
            << std::endl;

        return false;
    }

    std::cout
        << "[Predictor] Loading labels..."
        << std::endl;

    if (!neuralModel_.loadLabels(labelsFile))
    {
        std::cerr
            << "[Predictor] Failed loading labels."
            << std::endl;

        return false;
    }

    if (neuralModel_.inputSize() == 0)
    {
        std::cerr
            << "[Predictor] Invalid input size."
            << std::endl;

        return false;
    }

    if (neuralModel_.outputSize() == 0)
    {
        std::cerr
            << "[Predictor] Invalid output size."
            << std::endl;

        return false;
    }

    std::cout
        << "[Predictor] Rebuilding network..."
        << std::endl;

    neuralModel_.initialize(
        neuralModel_.inputSize(),
        neuralModel_.outputSize()
    );

    std::cout
        << "[Predictor] Loading model weights..."
        << std::endl;

    if (!neuralModel_.loadModel(modelFile))
    {
        std::cerr
            << "[Predictor] Failed loading model."
            << std::endl;

        return false;
    }

    loaded_ = true;

    std::cout
        << "[Predictor] Model ready."
        << std::endl;

    return true;
}

PredictionResult Predictor::predict(
    const std::string& text) const
{
    PredictionResult result;

    if (!loaded_)
    {
        return result;
    }

    if (text.empty())
    {
        return result;
    }

    const auto bow =
        vocabulary_.textToVector(text);

    if (bow.empty())
    {
        return result;
    }

    result =
        neuralModel_.predict(bow);

    return result;
}

bool Predictor::isReady() const noexcept
{
    return loaded_;
}

const Vocabulary&
Predictor::getVocabulary() const noexcept
{
    return vocabulary_;
}

const NeuralModel&
Predictor::getModel() const noexcept
{
    return neuralModel_;
}
