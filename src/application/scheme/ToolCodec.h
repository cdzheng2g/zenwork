#pragma once
#include "application/operators/OperatorRegistry.h"
#include <stdexcept>
namespace ToolCodec {
inline QJsonObject encode(const SchemeTool& stored)
{
    auto tool = stored;
    const auto error = OperatorRegistry::migrate(tool);
    if (!error.isEmpty()) throw std::runtime_error(error.toStdString());
    return { { "id", tool.id }, { "name", tool.name }, { "kind", tool.kind }, { "version", tool.version },
        { "parameters", tool.parameters }, { "rules", tool.rules } };
}
inline SchemeTool decode(const QJsonObject& value, const QJsonObject& legacyOcr = { })
{
    SchemeTool tool { value["id"].toString(), value["name"].toString(), value["kind"].toString() };
    const auto* d = OperatorRegistry::find(tool.kind);
    if (!d)
        throw std::runtime_error(("未知算子：" + tool.kind).toStdString());
    if ((value.contains("parameters") && !value["parameters"].isObject())
        || (value.contains("rules") && !value["rules"].isArray()))
        throw std::runtime_error("算子参数或规则格式无效");
    if (value.contains("version") && (!value["version"].isDouble()
        || value["version"].toDouble() != value["version"].toInt() || value["version"].toInt() < 1))
        throw std::runtime_error("不支持的算子版本");
    tool.version = value["version"].toInt(1);
    tool.parameters = value.contains("parameters") ? value["parameters"].toObject()
            : tool.kind == "ocr"     ? legacyOcr
                                     : QJsonObject { };
    const auto migrationError = OperatorRegistry::migrate(tool);
    if (!migrationError.isEmpty()) throw std::runtime_error(migrationError.toStdString());
    tool.parameters = parameterDefaults(d->parameters, tool.parameters);
    tool.rules = value.contains("rules") ? value["rules"].toArray()
        : d->defaultRules                ? d->defaultRules(tool.parameters)
                                         : QJsonArray { };
    const auto error = OperatorRegistry::validate(tool);
    if (!error.isEmpty())
        throw std::runtime_error(error.toStdString());
    return tool;
}
}
