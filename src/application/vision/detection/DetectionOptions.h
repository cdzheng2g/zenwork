#pragma once

#include "application/vision/ImageTransform.h"
#include "infrastructure/inference/rknn/RknnSession.h"

#include <QJsonObject>
#include <QStringList>

struct DetectionPreprocess {
    bool convertToRgb {true};
    ImageResizeMode resizeMode {ImageResizeMode::Letterbox};
    cv::Scalar padding {114, 114, 114};
    TensorInputType inputType {TensorInputType::UInt8};

    // FLOAT32 input = (pixel * scale - mean) / standardDeviation.
    double scale {1.0};
    cv::Scalar mean {0, 0, 0};
    cv::Scalar standardDeviation {1, 1, 1};
};

struct DetectionOptions {
    QString modelPath;
    QString decoderId;
    QJsonObject decoderParameters;
    DetectionPreprocess preprocess;
    QStringList labels;
    double confidenceThreshold {0.5};
    double nmsThreshold {0.45};
};
