#pragma once
#include <QJsonObject>
#include <QJsonArray>
#include <QList>
#include <QStringList>
#include <QMap>
#include <cmath>

enum class ParameterKind { Number, Boolean, Text, Choice, File };
struct ParameterSpec {
    QString key, label;
    ParameterKind kind { ParameterKind::Text };
    QJsonValue defaultValue;
    double minimum { 0 }, maximum { 0 }, step { 1 };
    int decimals { 0 };
    QString unit;
    QStringList choices;
    bool slider { false };
    QString fileFilter;
    bool advanced { false };
    QMap<QString, QString> choiceLabels;
};
using ParameterSchema = QList<ParameterSpec>;
inline ParameterSpec numberParameter(QString key, QString label, double value, double low, double high,
    int decimals = 0, double step = 1, QString unit = { }, bool slider = false)
{
    return { key, label, ParameterKind::Number, value, low, high, step, decimals, unit, { }, slider };
}
inline ParameterSpec boolParameter(QString key, QString label, bool value)
{
    return { key, label, ParameterKind::Boolean, value };
}
inline ParameterSpec textParameter(QString key, QString label, QString value = { })
{
    return { key, label, ParameterKind::Text, value };
}
inline ParameterSpec choiceParameter(QString key, QString label, QString value, QStringList choices,
    QMap<QString, QString> labels = {})
{
    auto spec = textParameter(key, label, value);
    spec.kind = ParameterKind::Choice;
    spec.choices = choices;
    spec.choiceLabels = std::move(labels);
    return spec;
}
inline ParameterSpec fileParameter(QString key, QString label, QString value = {},
    QString filter = {}, bool advanced = true)
{
    auto spec = textParameter(key, label, value);
    spec.kind = ParameterKind::File;
    spec.fileFilter = filter;
    spec.advanced = advanced;
    return spec;
}
inline QJsonObject parameterDefaults(const ParameterSchema& schema, QJsonObject values = { })
{
    for (const auto& p : schema)
        if (!values.contains(p.key))
            values[p.key] = p.defaultValue;
    return values;
}
inline QString validateParameters(const ParameterSchema& schema, const QJsonObject& stored)
{
    const auto values = parameterDefaults(schema, stored);
    for (const auto& p : schema) {
        const auto value = values.value(p.key);
        bool valid = false;
        switch (p.kind) {
        case ParameterKind::Number: {
            const double n = value.toDouble();
            valid = value.isDouble() && std::isfinite(n) && n >= p.minimum && n <= p.maximum
                && (p.decimals != 0 || n == std::floor(n));
            break;
        }
        case ParameterKind::Boolean:
            valid = value.isBool();
            break;
        case ParameterKind::Text:
        case ParameterKind::File:
            valid = value.isString();
            break;
        case ParameterKind::Choice:
            valid = value.isString() && p.choices.contains(value.toString());
            break;
        }
        if (!valid)
            return QStringLiteral("参数无效：%1").arg(p.label);
    }
    return { };
}
struct OutputField {
    QString key, label;
    ParameterKind kind;
    QString unit;
};
