#include "DetectionResults.h"
#include <algorithm>
#include <cmath>
#include <stdexcept>

std::vector<DetectedObject> suppressDetections(std::vector<DetectedObject> objects,
    double confidenceThreshold, double iouThreshold, std::size_t maxDetections)
{
    if (!std::isfinite(confidenceThreshold) || confidenceThreshold < 0 || confidenceThreshold > 1
        || !std::isfinite(iouThreshold) || iouThreshold < 0 || iouThreshold > 1 || maxDetections == 0)
        throw std::runtime_error("检测筛选阈值或数量上限无效");
    objects.erase(std::remove_if(objects.begin(), objects.end(), [&](const auto& object) {
        const auto& box = object.box;
        return !std::isfinite(object.confidence) || object.confidence < confidenceThreshold
            || object.confidence > 1 || !std::isfinite(box.x()) || !std::isfinite(box.y())
            || !std::isfinite(box.width()) || !std::isfinite(box.height()) || box.isEmpty();
    }), objects.end());
    std::stable_sort(objects.begin(), objects.end(), [](const auto& a, const auto& b) {
        return a.confidence > b.confidence;
    });
    std::vector<DetectedObject> selected;
    for (const auto& object : objects) {
        bool suppressed = false;
        for (const auto& previous : selected) {
            if (previous.classId != object.classId)
                continue;
            const auto intersection = previous.box.intersected(object.box);
            const double intersectionArea = intersection.isEmpty() ? 0 : intersection.width() * intersection.height();
            const double unionArea = previous.box.width() * previous.box.height()
                + object.box.width() * object.box.height() - intersectionArea;
            if (unionArea > 0 && intersectionArea / unionArea > iouThreshold) {
                suppressed = true;
                break;
            }
        }
        if (!suppressed) {
            selected.push_back(object);
            if (selected.size() == maxDetections)
                break;
        }
    }
    return selected;
}

QJsonArray detectionRegions(const std::vector<DetectedObject>& objects, const OperatorImageInput& input)
{
    QJsonArray regions;
    const QRectF bounds(0, 0, input.image.cols, input.image.rows);
    for (const auto& object : objects) {
        const auto box = object.box.intersected(bounds);
        if (box.isEmpty())
            continue;
        regions.append(QJsonObject { {"classId", object.classId}, {"label", object.label},
            {"confidence", object.confidence}, {"polygon", input.polygon(box)} });
    }
    return regions;
}
