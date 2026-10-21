#include "application/vision/ImageInput.h"
#include "application/vision/ImageTransform.h"
#include "application/operators/OperatorRegistry.h"
#include "application/vision/DetectionResults.h"
#include <QCoreApplication>
#include <cmath>
#include <cstdio>

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    int failures = 0;
    auto check = [&](bool ok, const char* label) {
        printf("[%s] %s\n", ok ? "PASS" : "FAIL", label);
        failures += !ok;
    };
    auto near = [](double a, double b) { return std::abs(a - b) < 1e-6; };
    InspectionRequest request;
    request.image = cv::Mat(80, 100, CV_8UC3, cv::Scalar(12, 34, 56));
    request.reference.referenceRect = QRectF(10, 20, 30, 40);
    OperatorRunContext context(request);
    context.alignedToImage = (cv::Mat_<double>(2, 3) << 0, -1, 90, 1, 0, 5);
    OperatorOutcome outcome;
    auto input = prepareOperatorImage(context, {}, outcome);
    const auto first = input->polygon(QRectF(0, 0, 10, 10))[0].toArray();
    check(input && input->image.cols == 30 && input->image.rows == 40
            && near(first[0].toDouble(), 70) && near(first[1].toDouble(), 15),
        "ROI offset is composed before the alignment transform");
    context.alignmentFailed = true;
    auto skipped = prepareOperatorImage(context, {}, outcome);
    check(!skipped && outcome.state == NodeState::Skipped, "upstream alignment failure skips dependent operators");
    ImageInputSpec raw;
    raw.alignedKey = "useAligned";
    auto original = prepareOperatorImage(context, {{"fullImage", true}, {"useAligned", false}}, outcome, raw);
    const auto rawFirst = original->polygon(QRectF(3, 4, 5, 6))[0].toArray();
    check(original->image.cols == 100 && rawFirst[0].toDouble() == 3 && rawFirst[1].toDouble() == 4,
        "raw-image operators ignore a failed alignment and its transform");
    context.alignmentFailed = false;
    bool rejected = false;
    try { prepareOperatorImage(context, {{"roi", QJsonArray{90, 70, 20, 20}}}, outcome); }
    catch (const std::exception&) { rejected = true; }
    check(rejected, "out-of-image ROI is rejected instead of silently clipped");
    auto tool = OperatorRegistry::makeTool("ocr");
    tool.parameters["roi"] = "0,0,10,10";
    check(!OperatorRegistry::validate(tool).isEmpty(), "registry uses the shared ROI type validator");

    const cv::Mat odd(37, 101, CV_8UC3, cv::Scalar(1, 2, 3));
    for (const auto mode : {ImageResizeMode::Letterbox, ImageResizeMode::Stretch}) {
        const auto prepared = resizeForModel(odd, {64, 64}, mode);
        const auto& m = prepared.toSource;
        const QRectF source(7, 9, 31, 12);
        const QRectF model((source.x() - m.at<double>(0, 2)) / m.at<double>(0, 0),
            (source.y() - m.at<double>(1, 2)) / m.at<double>(1, 1),
            source.width() / m.at<double>(0, 0), source.height() / m.at<double>(1, 1));
        const auto restored = prepared.sourceRect(model);
        check(prepared.image.cols == 64 && prepared.image.rows == 64
                && near(restored.x(), source.x()) && near(restored.y(), source.y())
                && near(restored.width(), source.width()) && near(restored.height(), source.height()),
            mode == ImageResizeMode::Letterbox ? "letterbox inverse uses actual rounded dimensions"
                                              : "stretch inverse retains independent axis scales");
    }
    const auto padded = resizeForModel(odd, {64, 64});
    check(padded.image.at<cv::Vec3b>(0, 0) == cv::Vec3b(114, 114, 114), "letterbox uses declared padding");
    const auto detections = suppressDetections({
        {0, "bolt", .9, QRectF(1, 2, 10, 10)},
        {0, "bolt", .8, QRectF(1, 2, 10, 10)},
        {1, "nut", .85, QRectF(1, 2, 10, 10)},
        {0, "low", .1, QRectF(30, 30, 10, 10)}}, .5, .45);
    check(detections.size() == 2 && detections[0].confidence == .9 && detections[1].classId == 1,
        "NMS removes weaker duplicate boxes without suppressing a different class");
    const auto overlays = detectionRegions(detections, *input);
    const auto firstCorner = overlays[0].toObject()["polygon"].toArray()[0].toArray();
    check(overlays.size() == 2 && firstCorner[0].toDouble() == 68 && firstCorner[1].toDouble() == 16,
        "detection overlays compose box coordinates, ROI offset and alignment");
    return failures ? 1 : 0;
}
