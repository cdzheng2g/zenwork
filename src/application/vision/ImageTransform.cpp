#include "ImageTransform.h"
#include <opencv2/imgproc.hpp>
#include <algorithm>
#include <cmath>
#include <stdexcept>

QRectF PreparedModelImage::sourceRect(const QRectF& rect) const
{
    return QRectF((rect.x() * toSource.at<double>(0, 0)) + toSource.at<double>(0, 2),
        (rect.y() * toSource.at<double>(1, 1)) + toSource.at<double>(1, 2),
        rect.width() * toSource.at<double>(0, 0), rect.height() * toSource.at<double>(1, 1));
}

PreparedModelImage resizeForModel(const cv::Mat& image, const cv::Size& size,
    ImageResizeMode mode, const cv::Scalar& padding)
{
    if (image.empty() || size.width <= 0 || size.height <= 0)
        throw std::runtime_error("模型输入图像或尺寸无效");
    cv::Size resized = size;
    int left = 0, top = 0;
    if (mode == ImageResizeMode::Letterbox) {
        const double scale = std::min(double(size.width) / image.cols, double(size.height) / image.rows);
        resized.width = std::clamp(int(std::round(image.cols * scale)), 1, size.width);
        resized.height = std::clamp(int(std::round(image.rows * scale)), 1, size.height);
        left = (size.width - resized.width) / 2;
        top = (size.height - resized.height) / 2;
    }
    cv::Mat scaled, result(size, image.type(), padding);
    cv::resize(image, scaled, resized);
    scaled.copyTo(result(cv::Rect(left, top, resized.width, resized.height)));
    const double sx = double(image.cols) / resized.width;
    const double sy = double(image.rows) / resized.height;
    return { result, (cv::Mat_<double>(2, 3) << sx, 0, -left * sx, 0, sy, -top * sy) };
}
