#include "DecisionTree.hpp"

#include <utility>

DecisionTree DecisionTree::createDefault(float minimumConfidence)
{
    auto fallback = std::make_unique<Node>();
    fallback->route = "clarify_request";
    fallback->reason = "Prediction confidence is below the decision threshold.";

    auto repeated = std::make_unique<Node>();
    repeated->route = "repeat_action";
    repeated->reason = "The same intent was requested again in the current conversation.";

    auto normal = std::make_unique<Node>();
    normal->reason = "Prediction accepted by the decision tree.";

    auto contextNode = std::make_unique<Node>();
    contextNode->predicate = [](const ToolRequest& request) {
        return request.historySize > 0 &&
               !request.previousIntent.empty() &&
               request.previousIntent == request.intent;
    };
    contextNode->whenTrue = std::move(repeated);
    contextNode->whenFalse = std::move(normal);

    auto root = std::make_unique<Node>();
    root->predicate = [minimumConfidence](const ToolRequest& request) {
        return request.confidence >= minimumConfidence;
    };
    root->whenTrue = std::move(contextNode);
    root->whenFalse = std::move(fallback);

    return DecisionTree(std::move(root));
}

DecisionTree::DecisionTree(std::unique_ptr<Node> root)
    : root_(std::move(root))
{
}

DecisionOutcome DecisionTree::evaluate(const ToolRequest& request) const
{
    DecisionOutcome outcome;
    if (!root_)
    {
        outcome.reason = "Decision tree is empty.";
        return outcome;
    }

    const Node* node = root_.get();
    while (node != nullptr && !node->isLeaf())
    {
        node = node->predicate(request) ? node->whenTrue.get() : node->whenFalse.get();
        ++outcome.depth;
    }

    if (node == nullptr)
    {
        outcome.reason = "Decision tree ended without a route.";
        return outcome;
    }

    outcome.accepted = true;
    outcome.route = node->route.empty() ? request.intent : node->route;
    outcome.reason = node->reason;
    return outcome;
}
