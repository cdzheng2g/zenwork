#pragma once

#include "application/vision/DetectionResults.h"
#include "infrastructure/inference/rknn/RknnSession.h"


// 解码上下文与统一输出接口。

struct DetectionDecodeContext {
    cv::Size modelSize;
    double confidenceThreshold {0.5};
    // Zero means no label dictionary was supplied.
    int expectedClasses {0};
};

enum class DetectionNmsPolicy {
    Required,
    AlreadyApplied
};

struct DecodedDetections {
    std::vector<DetectedObject> objects;

    DetectionNmsPolicy nmsPolicy {
        DetectionNmsPolicy::Required
    };
};

class DetectionDecoder {
public:
    virtual ~DetectionDecoder() = default;

    // 检查模型输出是否符合此解码器要求。
    virtual void validate(
        const std::vector<TensorInfo>& outputs) const = 0;

    // 将模型输出转换为统一目标列表。
    virtual DecodedDetections decode(
        const std::vector<TensorOutput>& outputs,
        const DetectionDecodeContext& context) const = 0;
};
