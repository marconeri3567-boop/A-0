#pragma once

#include "ContextStore.hpp"
#include "Vocabulary.hpp"
#include "NeuralModel.hpp"

#include <string>

class Predictor
{
public:
    Predictor() = default;
    bool load(const std::string& modelDir);
    [[nodiscard]] PredictionResult predict(const std::string& text) const;
    [[nodiscard]] bool isReady() const noexcept;
    [[nodiscard]] const Vocabulary& getVocabulary() const noexcept;
    [[nodiscard]] const NeuralModel& getModel() const noexcept;

private:
    Vocabulary vocabulary_;
    NeuralModel neuralModel_;
    bool loaded_ = false;
};
