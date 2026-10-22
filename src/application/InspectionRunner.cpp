#include "InspectionRunner.h"
#include "operators/OperatorRegistry.h"
#include "modules/rules/RuleRegistry.h"
#include "domain/project/LogicEvaluator.h"
#include "infrastructure/inference/rknn/RknnSession.h"

#include <QDateTime>
#include <QElapsedTimer>
#include <QJsonArray>
#include <stdexcept>

namespace {
QString status(NodeState state)
{
    switch (state) {
    case NodeState::Ok:
        return QStringLiteral("OK");
    case NodeState::Ng:
        return QStringLiteral("NG");
    case NodeState::Skipped:
        return QStringLiteral("SKIPPED");
    default:
        return QStringLiteral("ERROR");
    }
}
}

QJsonObject runInspection(const InspectionRequest& request)
{
    QElapsedTimer total;
    total.start();
    QJsonArray results;
    QHash<QUuid, NodeState> states;
    LogicExpression expression;
    expression.operation = request.settings.logicOr ? LogicOperator::Or : LogicOperator::And;
    OperatorRunContext context(request);
    bool recoveryRequired = false;

    for (const auto& tool : request.tools) {
        QElapsedTimer timer;
        timer.start();
        OperatorOutcome outcome;
        outcome.input = { { "source", QStringLiteral("相机原图") }, { "width", request.image.cols },
            { "height", request.image.rows } };
        try {
            if (request.image.empty())
                throw std::runtime_error("没有新鲜的相机帧");
            const auto* definition = OperatorRegistry::find(tool.kind);
            const auto executor = definition ? definition->execute : nullptr;
            if (!executor)
                throw std::runtime_error((QStringLiteral("不支持的工具类型: ") + tool.kind).toStdString());
            const auto invalid = OperatorRegistry::validate(tool);
            if (!invalid.isEmpty())
                throw std::runtime_error(invalid.toStdString());
            executor(context, parameterDefaults(definition->parameters, tool.parameters), outcome);
            if (outcome.state == NodeState::Ok) {
                bool passed = true;
                QString ruleError;
                const auto details = RuleRegistry::evaluate(tool.rules, outcome.fields, passed, ruleError);
                outcome.fields["judgements"] = details;
                if (!ruleError.isEmpty()) {
                    outcome.state = NodeState::Error;
                    outcome.error = ruleError;
                } else if (!passed)
                    outcome.state = NodeState::Ng;
            }
        } catch (const RknnTimeout& error) {
            outcome.state = NodeState::Error;
            outcome.error = QString::fromUtf8(error.what());
            recoveryRequired = true;
        } catch (const std::exception& error) {
            outcome.state = NodeState::Error;
            outcome.error = QString::fromUtf8(error.what());
            if (const auto* d = OperatorRegistry::find(tool.kind); d && d->producesAlignment)
                context.alignmentFailed = true;
        } catch (...) {
            outcome.state = NodeState::Error;
            outcome.error = QStringLiteral("算法执行异常");
            if (const auto* d = OperatorRegistry::find(tool.kind); d && d->producesAlignment)
                context.alignmentFailed = true;
        }
        const QUuid id = QUuid::createUuidV5(QUuid(), tool.id.toUtf8());
        states.insert(id, outcome.state);
        expression.terms.append(LogicTerm(id));
        results.append(QJsonObject { { "id", tool.id }, { "name", tool.name }, { "kind", tool.kind },
            { "status", status(outcome.state) }, { "input", outcome.input }, { "fields", outcome.fields },
            { "error", outcome.error }, { "elapsedMs", double(timer.elapsed()) } });
        if (recoveryRequired)
            break;
    }

    const auto final = LogicEvaluator::evaluate(expression, states);
    return { { "type", "scheme_result" }, { "scheme", request.scheme },
        { "timestamp", QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs) },
        { "logic", request.settings.logicOr ? "OR" : "AND" }, { "status", status(final) },
        { "elapsedMs", double(total.elapsed()) }, { "tools", results },
        { "recoveryRequired", recoveryRequired },
        { "error", recoveryRequired ? QStringLiteral("推理超时，检测已锁定，请重启应用恢复")
                                    : request.tools.isEmpty() ? QStringLiteral("方案没有工具") : QString() } };
}
