#pragma once

#include <QJsonArray>
#include <QRectF>
#include <opencv2/core.hpp>

QJsonArray operatorImagePolygon(const QRectF& rect, const cv::Mat& toImage);
