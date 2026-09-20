#include "Vocabulary.hpp"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

void Vocabulary::build(
    const std::vector<TrainingSample>& samples)
{
    tokens_.clear();
    tokenToIndex_.clear();

    for (const auto& sample : samples)
    {
        const auto tokens =
            tokenize(sample.input);

        for (const auto& token : tokens)
        {
            addToken(token);
        }
    }

    tokens_.shrink_to_fit();

    std::cout
        << "[Vocabulary] Vocabulary size: "
        << tokens_.size()
        << std::endl;
}

std::vector<std::string>
Vocabulary::tokenize(
    const std::string& text) const
{
    std::vector<std::string> tokens;

    const std::string normalized =
        normalize(text);

    std::istringstream stream(normalized);

    std::string token;

    while (stream >> token)
    {
        if (!token.empty())
        {
            tokens.emplace_back(std::move(token));
        }
    }

    return tokens;
}

std::string Vocabulary::normalize(
    const std::string& text) const
{
    std::string output;
    output.reserve(text.size());

    bool previousSpace = false;

    for (unsigned char ch : text)
    {
        if (std::isalnum(ch))
        {
            output.push_back(
                static_cast<char>(
                    std::tolower(ch)));

            previousSpace = false;
        }
        else
        {
            if (!previousSpace)
            {
                output.push_back(' ');
                previousSpace = true;
            }
        }
    }

    while (!output.empty() &&
           output.front() == ' ')
    {
        output.erase(output.begin());
    }

    while (!output.empty() &&
           output.back() == ' ')
    {
        output.pop_back();
    }

    return output;
}

std::vector<float>
Vocabulary::textToVector(
    const std::string& text) const
{
    std::vector<float> vector(
        tokens_.size(),
        0.0f);

    const auto tokens =
        tokenize(text);

    for (const auto& token : tokens)
    {
        const int index =
            getIndex(token);

        if (index >= 0)
        {
            vector[
                static_cast<std::size_t>(index)
            ] = 1.0f;
        }
    }

    return vector;
}

bool Vocabulary::contains(
    const std::string& token) const
{
    return tokenToIndex_.find(token)
           != tokenToIndex_.end();
}

int Vocabulary::getIndex(
    const std::string& token) const
{
    const auto it =
        tokenToIndex_.find(token);

    if (it == tokenToIndex_.end())
    {
        return -1;
    }

    return static_cast<int>(it->second);
}

std::string Vocabulary::getToken(
    std::size_t index) const
{
    if (index >= tokens_.size())
    {
        return {};
    }

    return tokens_[index];
}

std::size_t Vocabulary::size() const noexcept
{
    return tokens_.size();
}

bool Vocabulary::empty() const noexcept
{
    return tokens_.empty();
}

bool Vocabulary::save(
    const std::string& filePath) const
{
    try
    {
        json j;

        j["tokens"] = tokens_;

        std::ofstream file(filePath);

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
            << "[Vocabulary] Save error: "
            << ex.what()
            << std::endl;

        return false;
    }
}

bool Vocabulary::load(
    const std::string& filePath)
{
    try
    {
        std::ifstream file(filePath);

        if (!file.is_open())
        {
            return false;
        }

        json j;
        file >> j;

        if (!j.contains("tokens"))
        {
            return false;
        }

        if (!j["tokens"].is_array())
        {
            return false;
        }

        tokens_.clear();
        tokenToIndex_.clear();

        tokens_ =
            j["tokens"].get<
                std::vector<std::string>>();

        tokenToIndex_.reserve(
            tokens_.size());

        for (std::size_t i = 0;
             i < tokens_.size();
             ++i)
        {
            tokenToIndex_.emplace(
                tokens_[i],
                i);
        }

        return true;
    }
    catch (const std::exception& ex)
    {
        std::cerr
            << "[Vocabulary] Load error: "
            << ex.what()
            << std::endl;

        return false;
    }
}

const std::vector<std::string>&
Vocabulary::getTokens() const noexcept
{
    return tokens_;
}

void Vocabulary::addToken(
    const std::string& token)
{
    if (token.empty())
    {
        return;
    }

    const auto it =
        tokenToIndex_.find(token);

    if (it != tokenToIndex_.end())
    {
        return;
    }

    const std::size_t index =
        tokens_.size();

    tokens_.push_back(token);

    tokenToIndex_.emplace(
        token,
        index);
}
