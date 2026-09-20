#include "NeuralModel.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

NeuralModel::NeuralModel() = default;

void NeuralModel::initialize(
    std::size_t inputSize,
    std::size_t outputSize)
{
    inputSize_ = inputSize;
    outputSize_ = outputSize;

    network_ = tiny_dnn::network<tiny_dnn::sequential>();

    network_
        << tiny_dnn::fully_connected_layer(
               inputSize_,
               128)
        << tiny_dnn::relu_layer()
        << tiny_dnn::fully_connected_layer(
               128,
               64)
        << tiny_dnn::relu_layer()
        << tiny_dnn::fully_connected_layer(
               64,
               outputSize_)
        << tiny_dnn::softmax_layer(
               outputSize_);

    initialized_ = true;
}

bool NeuralModel::train(
    const std::vector<tiny_dnn::vec_t>& inputs,
    const std::vector<tiny_dnn::label_t>& labels,
    std::size_t epochs,
    std::size_t batchSize)
{
    if (!initialized_)
    {
        std::cerr
            << "[NeuralModel] Model not initialized."
            << std::endl;

        return false;
    }

    if (inputs.empty())
    {
        return false;
    }

    if (labels.empty())
    {
        return false;
    }

    if (inputs.size() != labels.size())
    {
        return false;
    }

    try
    {
        tiny_dnn::adam optimizer;

        optimizer.alpha = 0.001f;

        network_.train<
            tiny_dnn::cross_entropy_multiclass>(
                optimizer,
                inputs,
                labels,
                batchSize,
                epochs);

        return true;
    }
    catch (const std::exception& ex)
    {
        std::cerr
            << "[NeuralModel] Train error: "
            << ex.what()
            << std::endl;

        return false;
    }
}

PredictionResult NeuralModel::predict(
    const std::vector<float>& inputVector) const
{
    PredictionResult result;

    if (!initialized_)
    {
        return result;
    }

    if (inputVector.size() != inputSize_)
    {
        return result;
    }

    tiny_dnn::vec_t input(
        inputVector.begin(),
        inputVector.end());

    const tiny_dnn::vec_t probabilities =
        network_.predict(input);

    if (probabilities.empty())
    {
        return result;
    }

    const auto maxIt =
        std::max_element(
            probabilities.begin(),
            probabilities.end());

    const std::size_t predictedIndex =
        static_cast<std::size_t>(
            std::distance(
                probabilities.begin(),
                maxIt));

    result.intent =
        indexToIntent(predictedIndex);

    result.confidence =
        maxProbability(probabilities);

    return result;
}

bool NeuralModel::saveModel(
    const std::string& modelPath) const
{
    try
    {
        network_.save(modelPath);

        return true;
    }
    catch (const std::exception& ex)
    {
        std::cerr
            << "[NeuralModel] Save model error: "
            << ex.what()
            << std::endl;

        return false;
    }
}

bool NeuralModel::loadModel(
    const std::string& modelPath)
{
    try
    {
        network_.load(modelPath);

        initialized_ = true;

        return true;
    }
    catch (const std::exception& ex)
    {
        std::cerr
            << "[NeuralModel] Load model error: "
            << ex.what()
            << std::endl;

        return false;
    }
}

bool NeuralModel::saveLabels(
    const std::string& labelPath) const
{
    try
    {
        json j;

        j["input_size"] = inputSize_;
        j["output_size"] = outputSize_;
        j["intents"] = intents_;

        std::ofstream file(labelPath);

        if (!file.is_open())
        {
            return false;
        }

        file << j.dump(4);

        return true;
    }
    catch (const std::exception& ex)
    {
        std::cerr
            << "[NeuralModel] Save labels error: "
            << ex.what()
            << std::endl;

        return false;
    }
}

bool NeuralModel::loadLabels(
    const std::string& labelPath)
{
    try
    {
        std::ifstream file(labelPath);

        if (!file.is_open())
        {
            return false;
        }

        json j;
        file >> j;

        intents_.clear();
        intentToIndexMap_.clear();

        inputSize_ =
            j.at("input_size")
                .get<std::size_t>();

        outputSize_ =
            j.at("output_size")
                .get<std::size_t>();

        intents_ =
            j.at("intents")
                .get<std::vector<std::string>>();

        for (std::size_t i = 0;
             i < intents_.size();
             ++i)
        {
            intentToIndexMap_.emplace(
                intents_[i],
                i);
        }

        return true;
    }
    catch (const std::exception& ex)
    {
        std::cerr
            << "[NeuralModel] Load labels error: "
            << ex.what()
            << std::endl;

        return false;
    }
}

void NeuralModel::setIntents(
    const std::vector<std::string>& intents)
{
    intents_ = intents;

    intentToIndexMap_.clear();

    intentToIndexMap_.reserve(
        intents_.size());

    for (std::size_t i = 0;
         i < intents_.size();
         ++i)
    {
        intentToIndexMap_[
            intents_[i]
        ] = i;
    }
}

const std::vector<std::string>&
NeuralModel::getIntents() const noexcept
{
    return intents_;
}

std::size_t NeuralModel::classCount() const noexcept
{
    return intents_.size();
}

bool NeuralModel::isInitialized() const noexcept
{
    return initialized_;
}

std::size_t NeuralModel::inputSize() const noexcept
{
    return inputSize_;
}

std::size_t NeuralModel::outputSize() const noexcept
{
    return outputSize_;
}

int NeuralModel::intentToIndex(
    const std::string& intent) const
{
    const auto it =
        intentToIndexMap_.find(intent);

    if (it == intentToIndexMap_.end())
    {
        return -1;
    }

    return static_cast<int>(it->second);
}

std::string NeuralModel::indexToIntent(
    std::size_t index) const
{
    if (index >= intents_.size())
    {
        return {};
    }

    return intents_[index];
}

float NeuralModel::maxProbability(
    const tiny_dnn::vec_t& probabilities) const
{
    if (probabilities.empty())
    {
        return 0.0f;
    }

    return *std::max_element(
        probabilities.begin(),
        probabilities.end());
}
