#include "ui/pages/AlgorithmConfigPage.h"
#include "ui/components/ParameterForm.h"
#include "ui/components/NumericParameterField.h"
#include "application/operators/OperatorRegistry.h"
#include "ui/operators/OperatorEditorRegistry.h"
#include "ui/operators/ImageOperatorEditor.h"
#include "ui/components/FileParameterField.h"
#include "ui/ImageCanvasWidget.h"
#include <QApplication>
#include <QTemporaryDir>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <cstdio>

int main(int argc, char** argv)
{
    QTemporaryDir storage;
    qputenv("XDG_DATA_HOME", storage.path().toUtf8());
    QApplication app(argc, argv);
    int failures = 0;
    auto check = [&](bool ok, const char* label) {
        printf("[%s] %s\n", ok ? "PASS" : "FAIL", label);
        if (!ok)
            ++failures;
    };
    ParameterForm form(
        { numberParameter("gain", "Gain", 1, 0, 16, 2, .05, "", true), textParameter("text", "Text"),
            choiceParameter("mode", "Mode", "A", { "A", "B" }), boolParameter("flag", "Flag", true) });
    QJsonObject config { { "gain", 2.35 }, { "text", "00123" }, { "mode", "B" }, { "flag", false },
        { "futureField", 42 } };
    int changes = 0;
    QObject::connect(&form, &ParameterForm::changed, [&] { ++changes; });
    form.setConfiguration(config);
    check(form.configuration() == config && changes == 0,
        "loading a form preserves values and emits no user edit");
    form.findChild<NumericParameterField*>("gain")->setValue(4.15);
    check(changes == 1 && form.configuration()["gain"].toDouble() == 4.15,
        "numeric field emits a single edit without slider rounding loss");

    auto first = OperatorRegistry::makeTool("ocr"), second = OperatorRegistry::makeTool("ocr");
    first.parameters["roi"] = QJsonArray { 10, 20, 30, 40 };
    second.parameters["roi"] = QJsonArray { 100, 200, 60, 80 };
    first.parameters["recognitionModel"] = "first.rknn";
    second.parameters["recognitionModel"] = "second.rknn";
    auto disabled = first.rules[0].toObject();
    disabled["enabled"] = false;
    first.rules[0] = disabled;
    AlgorithmConfigPage page;
    page.setConfiguredTools({ first, second });
    auto saved = page.configuredTools();
    check(saved.size() == 2 && saved[0].parameters == first.parameters
            && saved[1].parameters == second.parameters,
        "actual operator page preserves two OCR instances and their distinct ROIs/resources");
    check(!saved[0].rules[0].toObject()["enabled"].toBool(true),
        "disabled rule remains visibly disabled after editing");
    auto* list = page.findChild<QListWidget*>("configuredOperatorList");
    list->setCurrentRow(1);
    list->setCurrentRow(0);
    check(page.configuredTools()[1].parameters == second.parameters,
        "selection changes do not overwrite other operator parameters");
    page.setConfiguredTools({ second });
    saved = page.configuredTools();
    check(saved.size() == 1 && saved[0].id == second.id && saved[0].parameters == second.parameters,
        "rebuilding editor list after removal preserves remaining instance");

    auto* ocrEditor = OperatorEditorRegistry::create("ocr", nullptr);
    check(dynamic_cast<ImageOperatorEditor*>(ocrEditor) != nullptr,
        "OCR uses the generic image editor without a custom factory");
    delete ocrEditor;
    auto* registeredDetection = OperatorEditorRegistry::create("detection", nullptr);
    check(dynamic_cast<ImageOperatorEditor*>(registeredDetection) != nullptr,
        "registered detection uses the shared image editor without a UI factory branch");
    const auto detection = OperatorRegistry::makeTool("detection");
    registeredDetection->setConfiguration(detection.parameters);
    check(registeredDetection->configuration() == detection.parameters
            && registeredDetection->findChild<FileParameterField*>("labelsPath"),
        "detection model, labels and decoder-owned settings round-trip through generated controls");
    delete registeredDetection;
    OperatorDefinition detector;
    detector.kind = "test_detection";
    detector.imageInput = ImageInputSpec {};
    detector.parameters = {boolParameter("fullImage", "Full image", true),
        fileParameter("modelPath", "Model", {}, "RKNN (*.rknn)"),
        textParameter("label", "Expected label"),
        numberParameter("confidence", "Confidence", .5, 0, 1, 2, .01)};
    ImageOperatorEditor generic(detector);
    const QJsonObject detectorConfig {{"fullImage", true}, {"modelPath", "/tmp/检测.rknn"},
        {"label", "bolt"}, {"confidence", .73}, {"roi", QJsonArray{3, 4, 20, 30}}, {"future", 9}};
    generic.setConfiguration(detectorConfig);
    check(generic.configuration() == detectorConfig && generic.findChild<FileParameterField*>("modelPath"),
        "new image operators reuse ROI, file picker and parameter persistence without custom UI");
    auto* mark = generic.findChild<QPushButton*>("markOperatorRoi");
    auto* canvas = generic.findChild<ImageCanvasWidget*>("operatorImagePreview");
    mark->setChecked(true);
    canvas->canvasPointClicked(QPointF(40, 50));
    canvas->canvasPointClicked(QPointF(10, 20));
    auto marked = generic.configuration();
    check(marked["roi"].toArray() == QJsonArray({10, 20, 30, 30}) && !marked["fullImage"].toBool()
            && marked["modelPath"] == detectorConfig["modelPath"],
        "ROI marking normalizes corners, selects region mode and preserves model configuration");
    generic.setConfiguration(detectorConfig);
    check(generic.configuration() == detectorConfig && !mark->isChecked(),
        "loading another instance clears unfinished marking without changing its parameters");
    return failures ? 1 : 0;
}
