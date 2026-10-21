#pragma once
#include <QJsonObject>
#include <opencv2/core.hpp>

QJsonObject runOcrEngine(const cv::Mat& bgr, const QJsonObject& config);
