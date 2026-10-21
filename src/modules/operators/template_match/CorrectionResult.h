#pragma once
#include <opencv2/core.hpp>
#include <QString>

struct CorrectionResult {
    bool success { false };
    QString message;

    cv::Mat correctedImage;
    cv::Mat affineMatrix; // 2x3 仿射矩阵(平移+旋转+缩放),行主序对应 cv::warpAffine 所需格式

    double dx { 0.0 };
    double dy { 0.0 };
    double angle { 0.0 }; // 角度制,正值表示逆时针
    double scale { 1.0 };

    double matchScore { 0.0 }; // 模板匹配的相关系数
};
