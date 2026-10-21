#include "application/InspectionRunner.h"
#include "application/operators/OperatorRegistry.h"
#include <QCoreApplication>
#include <QJsonArray>
#include <QJsonDocument>
#include <opencv2/imgproc.hpp>
#include <cstdio>
int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    int failures = 0;
    auto check = [&](bool value, const char* label) {
        printf("%s %s\n", value ? "PASS" : "FAIL", label);
        if (!value)
            ++failures;
    };
    InspectionRequest q;
    q.scheme = "test";
    q.tools = { { "11111111-1111-1111-1111-111111111111", "Template", "template" } };
    q.image = cv::Mat(240, 640, CV_8UC3, cv::Scalar(255, 255, 255));
    cv::putText(q.image, "ABC123", { 80, 145 }, cv::FONT_HERSHEY_SIMPLEX, 2.5, { 0, 0, 0 }, 4, cv::LINE_AA);
    q.reference.image = q.image.clone();
    q.reference.referenceRect = QRectF(65, 70, 350, 100);
    check(runInspection(q).value("status") == "OK", "real template matches identical image");
    const auto originalTools = q.tools;
    q.tools = { { "unsupported-tool", "Bad tool", "missing" }, { "template-tool", "Good tool", "template" } };
    check(runInspection(q).value("status") == "ERROR",
        "non-UUID legacy IDs cannot overwrite another operator state");
    q.tools = originalTools;
    cv::Mat transform = (cv::Mat_<double>(2, 3) << 1, 0, 24, 0, 1, 18);
    cv::warpAffine(q.reference.image, q.image, transform, q.reference.image.size(), cv::INTER_LINEAR,
        cv::BORDER_REPLICATE);
    auto shifted = runInspection(q);
    auto corners = shifted.value("tools")
                       .toArray()[0]
                       .toObject()
                       .value("fields")
                       .toObject()
                       .value("polygon")
                       .toArray();
    check(corners.size() == 4 && std::abs(corners[0].toArray()[0].toDouble() - 89) < 4
            && std::abs(corners[0].toArray()[1].toDouble() - 88) < 4,
        "template overlay maps to the translated input frame");
    q.image = cv::Mat(q.image.size(), CV_8UC3, cv::Scalar(255, 255, 255));
    check(runInspection(q).value("status") == "NG", "blank target is NG");
    q.image.release();
    check(runInspection(q).value("status") == "ERROR", "missing frame is explicit ERROR");
    q.image = q.reference.image.clone();
    q.reference.image.release();
    check(runInspection(q).value("status") == "ERROR", "missing reference is explicit ERROR");
    if (argc > 1) {
        QString dir = QString::fromLocal8Bit(argv[1]);
        q.tools = { { "22222222-2222-2222-2222-222222222222", "OCR", "ocr" } };
        q.tools[0].parameters = { { "recognitionModel", dir + "/ppocrv4_rec.rknn" },
            { "detectionModel", dir + "/ppocrv4_det.rknn" }, { "dictionary", dir + "/ppocr_keys_v1.txt" },
            { "autoRotate", true }, { "expectedText", "ABC123" } };
        q.tools[0].rules = OperatorRegistry::find("ocr")->defaultRules(q.tools[0].parameters);
        auto result = runInspection(q);
        puts(QJsonDocument(result).toJson(QJsonDocument::Compact).constData());
        check(result.value("status") == "OK", "NPU recognizes known text in ROI");
        auto regions = result.value("tools")
                           .toArray()[0]
                           .toObject()
                           .value("fields")
                           .toObject()
                           .value("regions")
                           .toArray();
        auto points = regions.isEmpty() ? QJsonArray() : regions[0].toObject().value("polygon").toArray();
        check(points.size() == 4 && points[0].toArray()[0].toDouble() == 65
                && points[0].toArray()[1].toDouble() == 70,
            "OCR overlay includes ROI offset in the input frame");
        q.tools[0].parameters["fullImage"] = true;
        result = runInspection(q);
        puts(QJsonDocument(result).toJson(QJsonDocument::Compact).constData());
        check(result.value("status") == "OK", "NPU detection plus recognition on full image");
        q.tools[0].parameters["fullImage"] = false;
        cv::rotate(q.image, q.image, cv::ROTATE_180);
        q.reference.referenceRect = QRectF(q.image.cols - 415, q.image.rows - 170, 350, 100);
        check(runInspection(q).value("status") == "OK", "upside-down text is recognized");
        q.tools[0].parameters["expectedText"] = "WRONG";
        q.tools[0].rules = OperatorRegistry::find("ocr")->defaultRules(q.tools[0].parameters);
        check(runInspection(q).value("status") == "NG", "OCR mismatch is NG");
        q.tools[0].parameters["recognitionModel"] = "/missing.rknn";
        check(runInspection(q).value("status") == "ERROR", "missing OCR model is ERROR");
    }
    return failures ? 1 : 0;
}
