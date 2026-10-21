#include "application/scheme/SchemePackage.h"
#include "application/scheme/SchemeCatalog.h"
#include "infrastructure/persistence/YamlDocument.h"
#include "ui/workbench/InspectionPresentation.h"
#include <QCoreApplication>
#include <QTemporaryDir>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QJsonArray>
#include <QStandardPaths>
#include <QSettings>
#include <opencv2/core.hpp>
#include <cstdio>

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    int failures = 0;
    auto check = [&](bool ok, const char* name) {
        std::printf("[%s] %s\n", ok ? "PASS" : "FAIL", name);
        if (!ok)
            ++failures;
    };
    QTemporaryDir source, target;
    QString error;
    qputenv("XDG_DATA_HOME", (target.path() + "/appdata").toUtf8());
    SchemePackage p;
    p.name = QStringLiteral("跨机 OCR 001");
    p.settings.referenceModel = "source-reference";
    p.settings.internalTriggerEnabled = false;
    p.settings.triggerIntervalMs = 1234;
    p.settings.logicOr = true;
    p.settings.communication = normalizedCommunicationSettings(
        { { "modbus_tcp_server", QJsonObject { { "port", 1602 }, { "unitId", 7 } } },
            { "modbus_tcp_client",
                QJsonObject { { "host", "192.168.1.99" }, { "port", 1503 }, { "unitId", 2 } } } });
    p.tools = { { "template-1", QStringLiteral("定位"), "template" },
        { "ocr-1", QStringLiteral("文字"), "ocr" } };
    p.tools[1].parameters = { { "fullImage", true }, { "expectedText", "00123" }, { "minConfidence", .85 } };
    p.tools[1].rules = OperatorRegistry::find("ocr")->defaultRules(p.tools[1].parameters);
    p.reference.image = cv::Mat(80, 100, CV_8UC3, cv::Scalar(20, 30, 40)).clone();
    p.reference.referenceRect = QRectF(10, 12, 45, 30);
    for (const QString& key :
        { QString("recognitionModel"), QString("detectionModel"), QString("dictionary") }) {
        const QString path = source.path() + "/" + key;
        QFile f(path);
        f.open(QIODevice::WriteOnly);
        f.write((key + "-test-bytes").toUtf8());
        f.close();
        p.tools[1].parameters[key] = path;
    }
    const QString exported = target.path() + "/portable.yaml";
    check(p.save(exported, &error, true), "export full YAML with reference and all OCR resources");
    const QString light = target.path() + "/light.yaml";
    check(p.save(light, &error), "export lightweight YAML and register shared models once");
    QJsonObject lightDocument;
    YamlDocument::read(light, lightDocument, &error);
    check(lightDocument.value("formatVersion").toInt() == 3
            && lightDocument.value("resourceMode").toString() == "shared"
            && !lightDocument.value("resources")
                .toObject()
                .value("ocr-1/recognitionModel")
                .toObject()
                .contains("base64"),
        "lightweight YAML contains hashes without embedded OCR models");
    check(QFileInfo(light).size() < QFileInfo(exported).size(),
        "lightweight export is smaller than full migration export");
    QFile textFile(exported);
    textFile.open(QIODevice::ReadOnly);
    const auto text = textFile.readAll();
    textFile.close();
    check(!text.contains(source.path().toUtf8()) && !text.contains("source-reference"),
        "export contains no source-machine paths or reference IDs");
    source.remove();
    SchemePackage lightweight;
    check(SchemePackage::load(light, lightweight, &error),
        "load lightweight YAML without source-machine paths");
    SchemeRuntimeSettings missing;
    check(!lightweight.installResources(target.path() + "/empty-device", missing, &error)
            && error.contains(QStringLiteral("缺少匹配")),
        "new device reports missing shared OCR model clearly");
    const QString shared
        = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/scheme_resources";
    const QString provisioned = target.path() + "/provisioned-device";
    QDir().mkpath(provisioned);
    for (auto it = lightweight.resourceHashes.begin(); it != lightweight.resourceHashes.end(); ++it) {
        const QString fileName = it.value() + "-"
            + (it.key().endsWith("/dictionary")            ? "ppocr_keys_v1.txt"
                    : it.key().endsWith("/detectionModel") ? "ppocrv4_det.rknn"
                                                           : "ppocrv4_rec.rknn");
        check(QFile::copy(shared + "/" + fileName, provisioned + "/" + fileName),
            "provision identical shared resource once");
    }
    SchemeRuntimeSettings linked;
    check(lightweight.installResources(provisioned, linked, &error),
        "lightweight scheme resolves preinstalled resources by checksum");
    SchemePackage imported;
    check(SchemePackage::load(exported, imported, &error), "import after source machine resources are gone");
    check(imported.name == p.name && imported.tools.size() == 2 && !imported.settings.internalTriggerEnabled
            && imported.settings.triggerIntervalMs == 1234 && imported.settings.logicOr,
        "tools, trigger and logic survive round trip");
    check(imported.tools[1].parameters.value("expectedText").toString() == "00123",
        "numeric OCR string keeps leading zeroes");
    check(imported.settings.communication == p.settings.communication,
        "server/client communication parameters survive portable export/import");
    lightweight.settings.modbusTriggerEnabled = true;
    lightweight.settings.outputChannels = QStringList { QStringLiteral("modbus_tcp_server") };
    check(lightweight.save(target.path() + "/modbus.yaml", &error)
            && SchemePackage::load(target.path() + "/modbus.yaml", lightweight, &error)
            && lightweight.settings.modbusTriggerEnabled
            && lightweight.settings.outputChannels.contains("modbus_tcp_server"),
        "Modbus trigger and communication survive portable YAML export/import");
    check(cv::norm(imported.reference.image, p.reference.image, cv::NORM_INF) == 0
            && imported.reference.referenceRect == p.reference.referenceRect,
        "reference pixels and ROI survive without loss");
    SchemeRuntimeSettings restored;
    check(imported.installResources(target.path() + "/new-machine", restored, &error),
        "restore models into a different machine directory");
    for (auto it = imported.resources.begin(); it != imported.resources.end(); ++it) {
        QFile f(imported.tools[1].parameters.value(it.key().section('/', -1)).toString());
        check(
            f.open(QIODevice::ReadOnly) && f.readAll() == it.value(), "restored resource bytes match source");
    }
    SchemeCatalog catalog(target.path() + "/catalog");
    check(catalog.saveComplete(imported.name, restored, imported.tools, &error),
        "save imported scheme transaction");
    SchemeCatalog reopened(target.path() + "/catalog");
    check(reopened.load(&error) && reopened.tools(imported.name).size() == 2, "restart loads YAML catalog");
    auto badTools = p.tools;
    badTools.append(p.tools.first());
    check(!catalog.saveComplete("bad", restored, badTools, &error) && !catalog.schemeNames().contains("bad"),
        "duplicate IDs rejected without partial catalog mutation");
    QJsonObject document;
    YamlDocument::read(exported, document, &error);
    auto resources = document.value("resources").toObject();
    auto resource = resources.value("ocr-1/dictionary").toObject();
    resource["sha256"] = "bad";
    resources["ocr-1/dictionary"] = resource;
    document["resources"] = resources;
    YamlDocument::write(target.path() + "/corrupt.yaml", document, &error);
    check(!SchemePackage::load(target.path() + "/corrupt.yaml", imported, &error),
        "corrupt resources rejected before import");
    YamlDocument::read(exported, document, &error);
    document["formatVersion"] = 99;
    YamlDocument::write(target.path() + "/future.yaml", document, &error);
    check(!SchemePackage::load(target.path() + "/future.yaml", imported, &error),
        "unknown format version rejected");
    QFile malformed(target.path() + "/malformed.yaml");
    malformed.open(QIODevice::WriteOnly);
    malformed.write("a: 1\na: 2\n");
    malformed.close();
    check(!YamlDocument::read(malformed.fileName(), document, &error), "duplicate YAML fields rejected");
    const QJsonObject tool { { "kind", "template" },
        { "input", QJsonObject { { "source", "定位校正图" }, { "width", 45 }, { "height", 30 } } },
        { "fields", QJsonObject { { "score", .91 }, { "dx", 3 }, { "dy", 4 }, { "angle", 2 } } } };
    check(InspectionPresentation::output(tool).contains("分数")
            && InspectionPresentation::output(tool).contains("角度"),
        "template output kept in result presentation");
    check(InspectionPresentation::input(tool).contains("45×30"), "tool input dimensions visible");
    QDir().mkpath(target.path() + "/legacy");
    QFile legacy(target.path() + "/legacy/catalog.json");
    legacy.open(QIODevice::WriteOnly);
    legacy.write(
        "{\"formatVersion\":1,\"currentScheme\":\"old\",\"schemes\":[{\"name\":\"old\",\"tools\":[]}]}");
    legacy.close();
    SchemeCatalog migrated(target.path() + "/legacy");
    QSettings legacyCommunication(
        QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/modbus_tcp_server.ini",
        QSettings::IniFormat);
    legacyCommunication.setValue("modbus/port", 1612);
    legacyCommunication.setValue("modbus/unitId", 4);
    legacyCommunication.sync();
    check(migrated.load(&error) && migrated.currentSchemeName() == "old" && migrated.save("old", &error)
            && QFile::exists(target.path() + "/legacy/catalog.yaml"),
        "legacy JSON migrates to YAML without losing schemes");
    check(migrated.settings("old").communication.value("modbus_tcp_server").toObject().value("port").toInt()
                == 1612
            && migrated.settings("old")
                    .communication.value("modbus_tcp_server")
                    .toObject()
                    .value("unitId")
                    .toInt()
                == 4,
        "legacy device communication parameters migrate into the old scheme");
    legacyCommunication.setValue("modbus/port", 1613);
    legacyCommunication.sync();
    check(migrated.load(&error)
            && migrated.settings("old")
                    .communication.value("modbus_tcp_server")
                    .toObject()
                    .value("port")
                    .toInt()
                == 1612,
        "migrated scheme parameters remain independent of later global INI changes");
    if (argc > 1) {
        SchemePackage actual;
        check(SchemePackage::load(QString::fromLocal8Bit(argv[1]), actual, &error),
            "load actual UI-exported scheme");
        SchemeRuntimeSettings actualSettings;
        check(actual.installResources(target.path() + "/actual", actualSettings, &error),
            "restore actual camera scheme resources in isolated directory");
        check(!actual.reference.image.empty() && actual.tools.size() > 0,
            "actual package contains reference and configured tools");
    }
    if (failures)
        std::fprintf(stderr, "Last error: %s\n", error.toUtf8().constData());
    return failures ? 1 : 0;
}
