#pragma once

#include "ToolDispatcher.hpp"

#include <cstddef>
#include <functional>
#include <memory>
#include <string>

/**
 * @brief Risultato della valutazione del decision tree.
 */
struct DecisionOutcome
{
    bool accepted = false;
    std::string route;
    std::string reason;
    std::size_t depth = 0;
};

/**
 * @brief Albero di decisione per il routing delle richieste del modello.
 *
 * Il classificatore neurale continua a produrre l'intento. Questo modulo
 * valuta invece confidenza e contesto prima di consentire l'esecuzione di un
 * tool, mantenendo separata la logica di decisione dalla rete neurale.
 */
class DecisionTree
{
public:
    using Predicate = std::function<bool(const ToolRequest&)>;

    DecisionTree() = default;

    /**
     * @brief Costruisce il tree standard usato dal flusso principale.
     *
     * Richieste poco affidabili vengono indirizzate a clarify_request; una
     * richiesta ripetuta nello stesso contesto viene indirizzata a
     * repeat_action. Tutte le altre conservano l'intento predetto.
     */
    [[nodiscard]] static DecisionTree createDefault(float minimumConfidence = 0.60F);

    [[nodiscard]] DecisionOutcome evaluate(const ToolRequest& request) const;

private:
    struct Node
    {
        Predicate predicate;
        std::string route;
        std::string reason;
        std::unique_ptr<Node> whenTrue;
        std::unique_ptr<Node> whenFalse;

        [[nodiscard]] bool isLeaf() const noexcept
        {
            return !whenTrue && !whenFalse;
        }
    };

    explicit DecisionTree(std::unique_ptr<Node> root);

    std::unique_ptr<Node> root_;
};
