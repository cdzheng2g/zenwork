#ifndef TEMPLATEMATCHCORRECTOR_H
#define TEMPLATEMATCHCORRECTOR_H

#include "CorrectionResult.h"
#include <QRectF>

#include <vector>

// 基于模板匹配的位置修正:模板取自基准图上用户手动框选的referenceRect区域
// (而不是自动取图像中心),适用于轮廓/外形规整、缺乏独特纹理特征的工件,
// 也让用户可以框出工件上最有代表性、区分度最高的局部区域,兼顾准确率和速度。
// 核心思路(参考多角度多尺度模板匹配方案):
//   1. 从基准图裁出referenceRect区域,在给定角度/尺度范围步长内批量生成
//      旋转+缩放模板(无损旋转:扩展画布到旋转后最小外接矩形,避免裁切黑边)
//   2. 对基准图裁块和实时图统一做Canny边缘提取(边缘特征比灰度/掩膜更准更快)
//   3. 用图像金字塔降采样(cv::resize,不用pyrDown)在低分辨率下搜索,加速匹配
//   4. matchTemplate(TM_CCOEFF_NORMED)逐模板匹配,注意不对结果矩阵做normalize,
//      否则会破坏不同模板之间的分数可比性
//   5. 收集所有高于分数阈值的匹配框,基于旋转矩形IoU做NMS去重
//   6. 取最优匹配框反推平移+旋转+缩放,warpAffine对齐
class TemplateMatchCorrector {
public:
    struct TemplateInfo {
        cv::Mat edgeImage; // Canny边缘图(旋转+缩放后,画布已扩展到最小外接矩形)
        double angle { 0.0 };
        double scale { 1.0 };
        cv::Size originalTemplateSize; // 缩放前的模板尺寸,用于还原偏移量
    };

    struct MatchCandidate {
        cv::RotatedRect rect;
        double score { 0.0 };
        double angle { 0.0 };
        double scale { 1.0 };
    };

    TemplateMatchCorrector();

    // referenceImage: 基准图(defect-free标准姿态图像)
    // liveImage: 待检测的实时采集图像
    // referenceRect: 基准图上用户手动框选的模板区域(图像像素坐标系),不可为空,
    //   否则无法确定拿哪一块局部区域去实时图里搜索
    // 返回校正结果:以referenceImage坐标系为基准,把liveImage对齐过去
    CorrectionResult correct(
        const cv::Mat& referenceImage, const cv::Mat& liveImage, const QRectF& referenceRect) const;

    void setAngleRange(double minAngle, double maxAngle, double step)
    {
        _minAngle = minAngle;
        _maxAngle = maxAngle;
        _angleStep = step;
    }
    void setScaleRange(double minScale, double maxScale, double step)
    {
        _minScale = minScale;
        _maxScale = maxScale;
        _scaleStep = step;
    }
    void setScoreThreshold(double threshold)
    {
        _scoreThreshold = threshold;
    }
    void setIouThreshold(double threshold)
    {
        _iouThreshold = threshold;
    }
    void setPyramidLevel(int level)
    {
        _pyramidLevel = level;
    }
    void setFastSearch(bool enabled)
    {
        _fastSearch = enabled;
    }

private:
    std::vector<TemplateInfo> buildTemplates(const cv::Mat& templateEdge) const;
    std::vector<MatchCandidate> matchAllTemplates(
        const cv::Mat& searchEdge, const std::vector<TemplateInfo>& templates) const;
    std::vector<MatchCandidate> nonMaxSuppression(std::vector<MatchCandidate> candidates) const;
    static double rotatedRectIou(const cv::RotatedRect& a, const cv::RotatedRect& b);

    double _minAngle { -40.0 };
    double _maxAngle { 40.0 };
    double _angleStep { 2.0 };
    double _minScale { 1.0 };
    double _maxScale { 1.0 };
    double _scaleStep { 0.0 };
    double _scoreThreshold { 0.3 };
    double _iouThreshold { 0.3 };
    int _pyramidLevel { 1 }; // 0=不降采样,N=缩小1/2^N搜索后按比例还原坐标
    bool _fastSearch { true };
    mutable cv::Mat _cachedCrop;
    mutable std::vector<double> _cachedParameters;
    mutable std::vector<TemplateInfo> _cachedTemplates;
};

#endif // TEMPLATEMATCHCORRECTOR_H
