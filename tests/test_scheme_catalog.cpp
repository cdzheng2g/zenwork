#include "application/scheme/SchemeCatalog.h"

#include <QTemporaryDir>

#include <cstdio>

namespace {
int failures = 0;
void check(bool value, const char* description)
{
    std::printf("[%s] %s\n", value ? "PASS" : "FAIL", description);
    if (!value)
        ++failures;
}
}

int main()
{
    QTemporaryDir temp;
    SchemeCatalog catalog(temp.path());
    QString error;

    check(catalog.schemeNames().isEmpty(), "a new catalog has no phantom scheme");
    check(!catalog.save(QString(), &error), "an unnamed scheme cannot be saved");
    check(catalog.save(QStringLiteral("瓶盖日期识别"), &error), "a named scheme saves");
    check(catalog.schemeNames() == QStringList { QStringLiteral("瓶盖日期识别") },
        "only saved scheme appears in catalog");
    check(catalog.currentSchemeName() == QStringLiteral("瓶盖日期识别"), "saved scheme becomes current");
    check(catalog.setTools(QStringLiteral("瓶盖日期识别"),
              { { QStringLiteral("ocr-1"), QStringLiteral("日期 OCR"), QStringLiteral("ocr") } }, &error),
        "a configured OCR tool saves with its scheme");

    SchemeCatalog restored(temp.path());
    SchemeRuntimeSettings config;
    config.logicOr = true;
    config.internalTriggerEnabled = false;
    config.triggerIntervalMs = 1250;
    config.referenceModel = "trial-reference";

    check(catalog.save(QStringLiteral("瓶盖日期识别"), config, &error), "save execution settings");
    check(restored.load(&error), "saved catalog reloads");
    const auto persisted = restored.settings(QStringLiteral("瓶盖日期识别"));
    check(!persisted.internalTriggerEnabled && persisted.triggerIntervalMs == 1250,
        "internal trigger mode and interval survive reload");
    check(persisted.logicOr && persisted.referenceModel == "trial-reference",
        "OR, reference and OCR settings survive reload");
    config.modbusTriggerEnabled = true;
    config.outputChannels = QStringList { QStringLiteral("modbus_tcp_server") };
    config.communication = normalizedCommunicationSettings(
        { { "modbus_tcp_server", QJsonObject { { "port", 1602 }, { "unitId", 7 } } } });
    check(catalog.save(QStringLiteral("瓶盖日期识别"), config, &error) && restored.load(&error)
            && restored.settings(QStringLiteral("瓶盖日期识别")).modbusTriggerEnabled
            && restored.settings(QStringLiteral("瓶盖日期识别")).outputChannels == config.outputChannels,
        "Modbus trigger and output survive a catalog reload");
    check(restored.settings(QStringLiteral("瓶盖日期识别")).communication == config.communication,
        "communication parameters are stored with the scheme");
    check(restored.schemeNames() == QStringList { QStringLiteral("瓶盖日期识别") },
        "saved scheme persists across reload");
    check(
        restored.tools(QStringLiteral("瓶盖日期识别")).size() == 1, "saved tool list persists across reload");
    check(restored.remove(QStringLiteral("瓶盖日期识别"), &error), "saved scheme can be deleted");
    check(restored.schemeNames().isEmpty(), "deleted scheme no longer appears in catalog");
    return failures == 0 ? 0 : 1;
}
