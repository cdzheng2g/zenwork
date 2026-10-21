#include "application/operators/OperatorRegistry.h"
#include "application/scheme/SchemeCodec.h"
#include "application/scheme/SchemePackage.h"
#include "modules/rules/RuleRegistry.h"
#include "infrastructure/persistence/YamlDocument.h"
#include "contracts/Trigger.h"
#include <QCoreApplication>
#include <QTemporaryDir>
#include <QFile>
#include <QJsonDocument>
#include <cstdio>
int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    QTemporaryDir tmp;
    int failures = 0;
    QString error;
    qputenv("XDG_DATA_HOME", tmp.path().toUtf8());
    auto check = [&](bool ok, const char* label) {
        printf("[%s] %s\n", ok ? "PASS" : "FAIL", label);
        if (!ok)
            ++failures;
    };
    auto a = OperatorRegistry::makeTool("ocr"), b = OperatorRegistry::makeTool("ocr");
    a.parameters["roi"] = QJsonArray { 1, 2, 30, 40 };
    b.parameters["roi"] = QJsonArray { 50, 60, 70, 80 };
    a.parameters["autoRotate"] = false;
    a.rules.append(QJsonObject { { "type", "text_equals" }, { "field", "text" },
        { "parameters", QJsonObject { { "expected", "00123" }, { "caseSensitive", true } } } });
    b.rules.append(QJsonObject { { "type", "text_equals" }, { "field", "text" },
        { "parameters", QJsonObject { { "expected", "OTHER" }, { "caseSensitive", false } } } });
    SchemeCatalog catalog(tmp.path() + "/catalog");
    check(catalog.saveComplete("two", { }, { a, b }, &error), "save two instances of the same operator");
    SchemeCatalog reopened(tmp.path() + "/catalog");
    check(reopened.load(&error), "load instance-based catalog");
    const auto tools = reopened.tools("two");
    check(tools.size() == 2 && tools[0].parameters == a.parameters && tools[1].parameters == b.parameters
            && tools[0].rules == a.rules && tools[1].rules == b.rules,
        "independent ROI, parameters and rules survive restart");
    bool passed;
    QString failure;
    auto results
        = RuleRegistry::evaluate(a.rules, { { "text", "00123" }, { "confidence", .9 } }, passed, failure);
    check(passed && failure.isEmpty() && results.size() == 4, "all OCR criteria pass without executing OCR");
    RuleRegistry::evaluate(b.rules, { { "text", "00123" }, { "confidence", .9 } }, passed, failure);
    check(!passed && failure.isEmpty(), "different instance criterion yields NG");
    RuleRegistry::evaluate(a.rules, { { "text", "00123" } }, passed, failure);
    check(!passed && !failure.isEmpty(), "missing output is ERROR, not silent numeric zero");
    auto invalid = a;
    invalid.parameters["autoRotate"] = "yes";
    check(!OperatorRegistry::validate(invalid).isEmpty(), "wrong parameter type rejected");
    invalid = a;
    invalid.parameters["roi"] = QJsonArray { 0, 0, -5, 20 };
    check(!OperatorRegistry::validate(invalid).isEmpty(),
        "module-specific ROI validator rejects invalid dimensions");
    bool rejected = false;
    try {
        auto encoded = ToolCodec::encode(a);
        encoded["version"] = 1.5;
        ToolCodec::decode(encoded);
    } catch (const std::exception&) {
        rejected = true;
    }
    check(rejected, "fractional operator version cannot silently downgrade");
    auto badSettings
        = SchemeCodec::decodeSettings({ { "communication", QJsonObject { { "tcp_server", false } } } });
    check(!SchemeCodec::validate(badSettings, { }).isEmpty(),
        "invalid communication block is rejected instead of replaced by defaults");
    invalid = a;
    invalid.rules.append(
        QJsonObject { { "type", "number_range" }, { "field", "text" }, { "parameters", QJsonObject { } } });
    check(!OperatorRegistry::validate(invalid).isEmpty(), "incompatible rule/output type rejected");
    invalid = a;
    auto rule = invalid.rules[1].toObject();
    rule["parameters"] = QJsonObject { { "minimum", 20 }, { "maximum", 1 } };
    invalid.rules[1] = rule;
    check(!OperatorRegistry::validate(invalid).isEmpty(), "inverted limits rejected before execution");
    auto legacy = SchemeCodec::encodeSettings({ });
    legacy["name"] = "legacy";
    legacy["ocr"]
        = QJsonObject { { "expectedText", "0001" }, { "minConfidence", .8 }, { "maxCharacters", 4 } };
    legacy["tools"] = QJsonArray { QJsonObject { { "id", a.id }, { "name", "old OCR" }, { "kind", "ocr" } } };
    YamlDocument::write(tmp.path() + "/catalog/catalog.yaml",
        { { "formatVersion", 1 }, { "currentScheme", "legacy" }, { "schemes", QJsonArray { legacy } } },
        &error);
    check(reopened.load(&error) && reopened.tools("legacy").first().rules.size() == 4,
        "legacy catalog migrates OCR criteria into explicit rules");
    const auto oldTool = reopened.tools("legacy").first();
    RuleRegistry::evaluate(oldTool.rules, { { "text", "0001" }, { "confidence", .85 } }, passed, failure);
    check(passed, "legacy leading-zero target retains behavior");
    RuleRegistry::evaluate(oldTool.rules, { { "text", "0001" }, { "confidence", .7 } }, passed, failure);
    check(!passed, "legacy minimum confidence retains behavior");
    auto bad = legacy;
    bad["tools"] = QJsonArray { QJsonObject { { "id", "x" }, { "name", "unknown" }, { "kind", "missing" } } };
    YamlDocument::write(tmp.path() + "/catalog/catalog.yaml",
        { { "formatVersion", 2 }, { "schemes", QJsonArray { bad } } }, &error);
    check(!reopened.load(&error) && reopened.currentSchemeName() == "legacy",
        "bad load leaves current catalog intact");
    InspectionGate gate { true, true, false, false, false, true, true, true };
    check(triggerDecision(TriggerSource::Modbus, gate, false, true) == TriggerDecision::Accepted,
        "Modbus uses common admission policy");
    check(triggerDecision(TriggerSource::Internal, gate, true, true) == TriggerDecision::Busy,
        "busy rejects all triggers");
    gate.editing = true;
    check(triggerDecision(TriggerSource::Manual, gate, false, true) == TriggerDecision::NotReady,
        "editing prevents manual trigger");
    gate.editing = false;
    gate.paused = true;
    check(triggerDecision(TriggerSource::Manual, gate, false, true) == TriggerDecision::Accepted
            && triggerDecision(TriggerSource::Modbus, gate, false, true) == TriggerDecision::NotReady,
        "manual testing remains available while automatic detection paused");
    if (failures)
        fprintf(stderr, "last error: %s\n", error.toUtf8().constData());
    return failures ? 1 : 0;
}
