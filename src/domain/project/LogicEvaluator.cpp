#include "LogicEvaluator.h"

LogicTerm::LogicTerm(const LogicExpression& expression)
    : expression(std::make_shared<LogicExpression>(expression))
{
}

namespace {
NodeState evaluateTerm(const LogicTerm& term, const QHash<QUuid, NodeState>& nodeStates)
{
    if (term.expression)
        return LogicEvaluator::evaluate(*term.expression, nodeStates);
    return nodeStates.value(term.nodeId, NodeState::Error);
}
}

NodeState LogicEvaluator::evaluate(
    const LogicExpression& expression, const QHash<QUuid, NodeState>& nodeStates)
{
    if (expression.terms.isEmpty())
        return NodeState::Error;

    bool hasNg = false;
    bool hasOk = false;
    for (const LogicTerm& term : expression.terms) {
        const NodeState state = evaluateTerm(term, nodeStates);
        if (state == NodeState::Error)
            return NodeState::Error;
        hasOk = hasOk || state == NodeState::Ok;
        hasNg = hasNg || state != NodeState::Ok;
    }

    if (expression.operation == LogicOperator::And)
        return hasNg ? NodeState::Ng : NodeState::Ok;
    return hasOk ? NodeState::Ok : NodeState::Ng;
}
