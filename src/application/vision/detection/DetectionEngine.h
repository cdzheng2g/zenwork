#pragma once
#include "DetectionDecoderRegistry.h"
#include "DetectionOptions.h"

// One instance per execution thread, matching RknnSession ownership.
class DetectionEngine final
{
public:
    std::vector<DetectedObject> detect(const cv::Mat& bgr, const DetectionOptions& options);

private:
    QString _decoderId;
    QJsonObject _decoderParameters;
    std::unique_ptr<DetectionDecoder> _decoder;
};
