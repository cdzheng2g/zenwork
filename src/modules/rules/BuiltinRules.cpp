#include "BuiltinRules.h"
namespace {
QString validateRange(const QJsonObject& p)
{
    return p["minimum"].toDouble() > p["maximum"].toDouble() ? QStringLiteral("判断下限不能大于上限")
                                                             : QString();
}
}
QList<RuleDefinition> builtinRules()
{
    return { { "text_not_empty", "文字非空", ParameterKind::Text, { },
                 [](const QJsonValue& v, const QJsonObject&) {
                     return RuleOutcome { !v.toString().isEmpty(), { }, "文字不能为空", v };
                 } },
        { "text_length", "文字长度", ParameterKind::Text,
            { numberParameter("minimum", "最少字符", 0, 0, 4096),
                numberParameter("maximum", "最多字符", 32, 0, 4096) },
            [](const QJsonValue& v, const QJsonObject& p) {
                if (p["minimum"].toInt() > p["maximum"].toInt())
                    return RuleOutcome { false, "最少字符不能大于最多字符", { }, v };
                const int n = v.toString().size();
                return RuleOutcome { n >= p["minimum"].toInt() && n <= p["maximum"].toInt(), { },
                    "文字长度超出范围", n };
            },
            validateRange },
        { "text_equals", "文字相等", ParameterKind::Text,
            { textParameter("expected", "期望文字"), boolParameter("caseSensitive", "区分大小写", false) },
            [](const QJsonValue& v, const QJsonObject& p) {
                return RuleOutcome { v.toString().compare(p["expected"].toString(),
                                         p["caseSensitive"].toBool() ? Qt::CaseSensitive
                                                                     : Qt::CaseInsensitive)
                        == 0,
                    { }, "文字与期望值不一致", v };
            } },
        { "number_range", "数值范围", ParameterKind::Number,
            { numberParameter("minimum", "下限", 0, -1e12, 1e12, 4, .01),
                numberParameter("maximum", "上限", 1, -1e12, 1e12, 4, .01) },
            [](const QJsonValue& v, const QJsonObject& p) {
                if (p["minimum"].toDouble() > p["maximum"].toDouble())
                    return RuleOutcome { false, "下限不能大于上限", { }, v };
                return RuleOutcome { v.toDouble() >= p["minimum"].toDouble()
                        && v.toDouble() <= p["maximum"].toDouble(),
                    { }, "数值超出范围", v };
            },
            validateRange } };
}
