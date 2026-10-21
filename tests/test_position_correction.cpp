// 位置修正模块的最小可运行验证:构造一张有纹理的合成基准图,对其施加已知的
// 旋转+平移变换生成"实时图",在基准图中心手动框选一块模板区域,跑模板匹配,
// 检查修正后的图像能否与基准图重新对齐。不依赖任何测试框架,失败时返回非0。

#include "modules/operators/template_match/TemplateMatchCorrector.h"

#include <opencv2/imgproc.hpp>

#include <cstdio>

namespace {
int g_failureCount = 0;

void checkTrue(bool condition, const char* description)
{
    if (!condition) {
        std::printf("[FAIL] %s\n", description);
        ++g_failureCount;
    } else {
        std::printf("[PASS] %s\n", description);
    }
}

// 生成一张带丰富纹理(角点+边缘)的合成基准图,供Canny边缘模板匹配使用
cv::Mat buildSyntheticReferenceImage()
{
    cv::Mat image(600, 600, CV_8UC3, cv::Scalar(255, 255, 255));

    unsigned int seed = 12345;
    for (int i = 0; i < 40; ++i) {
        seed = seed * 1103515245u + 12345u;
        int cx = 150 + static_cast<int>(seed % 300u);
        seed = seed * 1103515245u + 12345u;
        int cy = 150 + static_cast<int>(seed % 300u);
        seed = seed * 1103515245u + 12345u;
        int size = 10 + static_cast<int>(seed % 25u);
        seed = seed * 1103515245u + 12345u;
        int gray = 30 + static_cast<int>(seed % 180u);

        if (i % 2 == 0) {
            cv::rectangle(image, cv::Rect(cx - size, cy - size, size * 2, size * 2),
                cv::Scalar(gray, gray, gray), cv::FILLED);
        } else {
            cv::circle(image, cv::Point(cx, cy), size, cv::Scalar(gray, gray, gray), cv::FILLED);
        }
    }
    return image;
}

// 用给定的角度(度)和平移量,在同尺寸画布上生成"实时图":live = warpAffine(ref, M)
// M以画布中心为旋转中心,先旋转再平移,不缩放。
cv::Mat applyKnownTransform(const cv::Mat& reference, double angleDeg, double tx, double ty)
{
    cv::Point2f center(reference.cols / 2.0f, reference.rows / 2.0f);
    cv::Mat rotMat = cv::getRotationMatrix2D(center, angleDeg, 1.0);
    rotMat.at<double>(0, 2) += tx;
    rotMat.at<double>(1, 2) += ty;

    cv::Mat live;
    cv::warpAffine(reference, live, rotMat, reference.size(), cv::INTER_LINEAR, cv::BORDER_REPLICATE);
    return live;
}

// 归一化互相关,衡量修正后的图像与基准图的对齐/相似程度(1.0为完全一致)。
// 只比较中心区域,避开warpAffine边界处BORDER_REPLICATE产生的拉伸伪影。
double correlationScore(const cv::Mat& a, const cv::Mat& b)
{
    cv::Rect roi(a.cols / 4, a.rows / 4, a.cols / 2, a.rows / 2);

    cv::Mat grayA;
    cv::Mat grayB;
    cv::cvtColor(a(roi), grayA, cv::COLOR_BGR2GRAY);
    cv::cvtColor(b(roi), grayB, cv::COLOR_BGR2GRAY);

    cv::Mat result;
    cv::matchTemplate(grayA, grayB, result, cv::TM_CCOEFF_NORMED);
    return result.at<float>(0, 0);
}
} // namespace

int main()
{
    const double appliedAngle = 8.0;
    const double appliedTx = 18.0;
    const double appliedTy = -22.0;

    cv::Mat referenceImage = buildSyntheticReferenceImage();
    cv::Mat liveImage = applyKnownTransform(referenceImage, appliedAngle, appliedTx, appliedTy);

    // 模拟用户在"基准图管理"里手动框选的模板区域:取图像中心一块纹理丰富的区域
    QRectF referenceRect(150.0, 150.0, 300.0, 300.0);

    TemplateMatchCorrector corrector;
    corrector.setAngleRange(-15.0, 15.0, 1.0);
    corrector.setScaleRange(1.0, 1.0, 0.0);
    corrector.setScoreThreshold(0.4);
    corrector.setPyramidLevel(1);

    CorrectionResult result = corrector.correct(referenceImage, liveImage, referenceRect);
    std::printf("[TemplateMatch] success=%d message=%s dx=%.2f dy=%.2f angle=%.2f scale=%.3f score=%.3f\n",
        result.success, result.message.toStdString().c_str(), result.dx, result.dy, result.angle,
        result.scale, result.matchScore);

    checkTrue(result.success, "TemplateMatch: correction reports success");
    if (result.success) {
        double score = correlationScore(result.correctedImage, referenceImage);
        std::printf("  corrected-vs-reference correlation = %.3f\n", score);
        checkTrue(score > 0.5, "TemplateMatch: corrected image roughly matches reference (corr > 0.5)");
    }

    if (g_failureCount == 0) {
        std::printf("\nAll checks passed.\n");
    } else {
        std::printf("\n%d check(s) failed.\n", g_failureCount);
    }
    return g_failureCount == 0 ? 0 : 1;
}
