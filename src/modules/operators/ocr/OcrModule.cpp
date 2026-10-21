#include "OcrModule.h"
#include "OcrOperator.h"
OperatorDefinition ocrDefinition()
{
    OperatorDefinition d;
    d.kind = "ocr";
    d.name = "OCR 字符识别";
    d.symbol = "文";
    d.imageInput = ImageInputSpec {};
    d.parameters = { boolParameter("fullImage", "整图识别", false),
        boolParameter("autoRotate", "识别倒置文字", true),
        fileParameter("recognitionModel", "识别模型", {}, "RKNN 模型 (*.rknn)"),
        fileParameter("detectionModel", "检测模型", {}, "RKNN 模型 (*.rknn)"),
        fileParameter("dictionary", "字符字典", {}, "字符字典 (*.txt)") };
    d.outputs = { { "text", "识别文字", ParameterKind::Text, { } },
        { "confidence", "置信度", ParameterKind::Number, { } } };
    d.resources = { { "recognitionModel", "models/ocr/ppocrv4_rec.rknn", { } },
        { "detectionModel", "models/ocr/ppocrv4_det.rknn", "fullImage" },
        { "dictionary", "models/ocr/ppocr_keys_v1.txt", { } } };
    d.execute = &executeOcrOperator;
    d.defaultRules = [](const QJsonObject& legacy) {
        QJsonArray rules { QJsonObject { { "type", "text_not_empty" }, { "field", "text" },
                               { "parameters", QJsonObject { } } },
            QJsonObject { { "type", "text_length" }, { "field", "text" },
                { "parameters",
                    QJsonObject {
                        { "minimum", 0 }, { "maximum", legacy.value("maxCharacters").toInt(32) } } } },
            QJsonObject { { "type", "number_range" }, { "field", "confidence" },
                { "parameters",
                    QJsonObject {
                        { "minimum", legacy.value("minConfidence").toDouble(.5) }, { "maximum", 1 } } } } };
        if (!legacy["expectedText"].toString().isEmpty())
            rules.append(QJsonObject { { "type", "text_equals" }, { "field", "text" },
                { "parameters",
                    QJsonObject { { "expected", legacy["expectedText"] },
                        { "caseSensitive", legacy["caseSensitive"].toBool() } } } });
        return rules;
    };
    return d;
}
