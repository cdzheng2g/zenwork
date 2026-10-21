#pragma once

#include <QHash>
#include <QList>
#include <QUuid>

#include <memory>

enum class NodeState { Ok, Ng, Error, Skipped };

enum class LogicOperator { And, Or };

struct LogicExpression;

struct LogicTerm {
    LogicTerm(const QUuid& nodeId)
        : nodeId(nodeId)
    {
    }
    LogicTerm(const LogicExpression& expression);

    QUuid nodeId;
    std::shared_ptr<LogicExpression> expression;
};

struct LogicExpression {
    LogicOperator operation { LogicOperator::And };
    QList<LogicTerm> terms;
};

class LogicEvaluator {
public:
    static NodeState evaluate(const LogicExpression& expression, const QHash<QUuid, NodeState>& nodeStates);
};
