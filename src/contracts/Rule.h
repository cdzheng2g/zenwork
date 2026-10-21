#pragma once
#include "ParameterSpec.h"
#include <functional>

struct RuleOutcome {
    bool passed { false };
    QString error, reason;
    QJsonValue actual;
};
struct RuleDefinition {
    QString type, name;
    ParameterKind inputKind;
    ParameterSchema parameters;
    std::function<RuleOutcome(const QJsonValue&, const QJsonObject&)> evaluate;
    std::function<QString(const QJsonObject&)> validate;
};
