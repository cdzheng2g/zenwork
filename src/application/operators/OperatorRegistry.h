#pragma once
#include "OperatorExecution.h"
#include "contracts/ParameterSpec.h"
#include "contracts/SchemeTool.h"
#include "contracts/ImageInputSpec.h"
#include <functional>
#include <optional>
struct ResourceSpec {
    QString key, defaultFile, enabledBy;
};
struct OperatorDefinition {
    QString kind, name, symbol;
    ParameterSchema parameters;
    QList<OutputField> outputs;
    QList<ResourceSpec> resources;
    OperatorExecutor execute { nullptr };
    std::function<QJsonArray(const QJsonObject&)> defaultRules;
    bool producesAlignment { false };
    bool allowsMultiple { true };
    std::function<QString(const QJsonObject&)> validate;
    std::optional<ImageInputSpec> imageInput;
};
class OperatorRegistry {
public:
    static const QList<OperatorDefinition>& definitions();
    static const OperatorDefinition* find(const QString& kind);
    static SchemeTool makeTool(const QString& kind);
    static QString validate(const SchemeTool& tool);
};
