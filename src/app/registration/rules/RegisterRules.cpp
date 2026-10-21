#include "modules/rules/RuleRegistry.h"
#include "modules/rules/BuiltinRules.h"
const QList<RuleDefinition>& RuleRegistry::definitions()
{
    static const auto rules = builtinRules();
    return rules;
}
