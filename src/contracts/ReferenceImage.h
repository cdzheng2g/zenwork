#pragma once
#include <opencv2/core.hpp>
#include <QString>
#include <QRectF>

struct ReferenceImage {
    QString modelName;
    QString imagePath;
    cv::Mat image;
    QRectF referenceRect; // 基准图上的关注区域/轮廓包围盒,同时也是模板匹配的模板区域
};
