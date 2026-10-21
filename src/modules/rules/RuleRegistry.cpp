#include "RuleRegistry.h"
const RuleDefinition* RuleRegistry::find(const QString& type)
{
    for (const auto& rule : definitions())
        if (rule.type == type)
            return &rule;
    return nullptr;
}
QString RuleRegistry::validate(const QJsonArray& rules, const QList<OutputField>& outputs)
{
    for (const auto& value : rules) {
        if (!value.isObject())
            return "判断规则格式无效";
        const auto rule = value.toObject();
        const auto* d = find(rule["type"].toString());
        if (!d)
            return "未知判断规则：" + rule["type"].toString();
        if (!rule["parameters"].isObject() || (rule.contains("enabled") && !rule["enabled"].isBool()))
            return "判断参数格式无效";
        bool fieldFound = false;
        for (const auto& f : outputs)
            if (f.key == rule["field"].toString() && f.kind == d->inputKind)
                fieldFound = true;
        if (!fieldFound)
            return "判断字段不存在或类型不匹配：" + rule["field"].toString();
        const auto p = parameterDefaults(d->parameters, rule["parameters"].toObject());
        const auto error = validateParameters(d->parameters, p);
        if (!error.isEmpty())
            return error;
        if (d->validate) {
            const auto customError = d->validate(p);
            if (!customError.isEmpty())
                return customError;
        }
    }
    return { };
}
QJsonArray RuleRegistry::evaluate(
    const QJsonArray& rules, const QJsonObject& fields, bool& passed, QString& error)
{
    QJsonArray results;
    passed = true;
    error.clear();
    for (const auto& value : rules) {
        const auto rule = value.toObject();
        if (!rule["enabled"].toBool(true))
            continue;
        const auto* d = find(rule["type"].toString());
        const auto actual = fields.value(rule["field"].toString());
        RuleOutcome outcome;
        if (!d)
            outcome.error = "未知判断规则";
        else if ((d->inputKind == ParameterKind::Text && !actual.isString())
            || (d->inputKind == ParameterKind::Number
                && (!actual.isDouble() || !std::isfinite(actual.toDouble()))))
            outcome.error = "输出字段缺失或类型不符";
        else
            outcome = d->evaluate(actual, parameterDefaults(d->parameters, rule["parameters"].toObject()));
        passed = passed && outcome.passed && outcome.error.isEmpty();
        if (!outcome.error.isEmpty())
            error = outcome.error;
        results.append(QJsonObject { { "type", rule["type"] }, { "field", rule["field"] },
            { "status",
                !outcome.error.isEmpty() ? "ERROR"
                    : outcome.passed     ? "OK"
                                         : "NG" },
            { "actual", outcome.actual }, { "expected", rule["parameters"] },
            { "reason", outcome.passed ? QString() : outcome.reason }, { "error", outcome.error } });
    }
    return results;
}
