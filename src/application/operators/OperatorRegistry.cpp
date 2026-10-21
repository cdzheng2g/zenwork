#include "OperatorRegistry.h"
#include "modules/rules/RuleRegistry.h"
#include "application/vision/ImageInput.h"
#include <QUuid>
const OperatorDefinition* OperatorRegistry::find(const QString& kind)
{
    for (const auto& d : definitions())
        if (d.kind == kind)
            return &d;
    return nullptr;
}
SchemeTool OperatorRegistry::makeTool(const QString& kind)
{
    const auto* d = find(kind);
    if (!d)
        return { };
    SchemeTool tool { QUuid::createUuid().toString(QUuid::WithoutBraces), d->name, kind };
    tool.parameters = parameterDefaults(d->parameters);
    if (d->defaultRules)
        tool.rules = d->defaultRules(tool.parameters);
    return tool;
}
QString OperatorRegistry::validate(const SchemeTool& tool)
{
    const auto* d = find(tool.kind);
    if (!d || tool.version != 1 || tool.id.isEmpty() || tool.name.isEmpty())
        return "算子类型、版本或标识无效";
    const auto error = validateParameters(d->parameters, tool.parameters);
    if (!error.isEmpty())
        return error;
    if (d->imageInput) {
        const auto inputError = validateImageInput(tool.parameters, *d->imageInput);
        if (!inputError.isEmpty())
            return inputError;
    }
    if (d->validate) {
        const auto customError = d->validate(parameterDefaults(d->parameters, tool.parameters));
        if (!customError.isEmpty())
            return customError;
    }
    return RuleRegistry::validate(tool.rules, d->outputs);
}
