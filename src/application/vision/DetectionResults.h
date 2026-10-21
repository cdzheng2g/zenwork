#pragma once
#include "ImageInput.h"
#include <vector>


struct DetectedObject {
    int classId { 0 };
    QString label;
    double confidence { 0 };
    QRectF box;
};



std::vector<DetectedObject> suppressDetections(std::vector<DetectedObject> objects,
    double confidenceThreshold, double iouThreshold, std::size_t maxDetections = 300);
QJsonArray detectionRegions(const std::vector<DetectedObject>& objects, const OperatorImageInput& input);


