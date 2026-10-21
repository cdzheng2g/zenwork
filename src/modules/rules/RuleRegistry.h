#pragma once
#include "contracts/Rule.h"
class RuleRegistry {
public:
    static const QList<RuleDefinition>& definitions();
    static const RuleDefinition* find(const QString& type);
    static QString validate(const QJsonArray& rules, const QList<OutputField>& outputs);
    static QJsonArray evaluate(
        const QJsonArray& rules, const QJsonObject& fields, bool& passed, QString& error);
};
