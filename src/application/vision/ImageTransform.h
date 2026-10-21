#pragma once
#include <QRectF>
#include <opencv2/core.hpp>

enum class ImageResizeMode { Stretch, Letterbox };
struct PreparedModelImage {
    cv::Mat image;
    cv::Mat toSource;
    QRectF sourceRect(const QRectF& modelRect) const;
};

PreparedModelImage resizeForModel(const cv::Mat& image, const cv::Size& size,
    ImageResizeMode mode = ImageResizeMode::Letterbox,
    const cv::Scalar& padding = cv::Scalar(114, 114, 114));
