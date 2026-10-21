#include "modules/operators/template_match/TemplateMatchCorrector.h"

#include <opencv2/imgproc.hpp>

#include <algorithm>
#include <cmath>

namespace {
cv::Mat toGray(const cv::Mat& image)
{
    if (image.channels() == 1) {
        return image;
    }
    cv::Mat gray;
    cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
    return gray;
}

cv::Mat toEdge(const cv::Mat& image)
{
    cv::Mat gray = toGray(image);
    cv::Mat blurred;
    cv::GaussianBlur(gray, blurred, cv::Size(3, 3), 0);
    cv::Mat edge;
    cv::Canny(blurred, edge, 50, 150);
    return edge;
}

// 无损旋转:画布先扩展到旋转后最小外接矩形再warpAffine,避免旋转后角部被裁掉,
// 同时比"按对角线扩成正方形"更省计算量。
cv::Mat rotateNoCrop(const cv::Mat& src, double angleDeg, double scale)
{
    cv::Point2f center(src.cols / 2.0f, src.rows / 2.0f);
    cv::Mat rotMat = cv::getRotationMatrix2D(center, angleDeg, scale);

    cv::RotatedRect bbox(center,
        cv::Size2f(src.cols * static_cast<float>(scale), src.rows * static_cast<float>(scale)),
        static_cast<float>(angleDeg));
    cv::Rect boundingRect = bbox.boundingRect();

    rotMat.at<double>(0, 2) += boundingRect.width / 2.0 - center.x;
    rotMat.at<double>(1, 2) += boundingRect.height / 2.0 - center.y;

    cv::Mat rotated;
    cv::warpAffine(src, rotated, rotMat, boundingRect.size());
    return rotated;
}
} // namespace

TemplateMatchCorrector::TemplateMatchCorrector() = default;

std::vector<TemplateMatchCorrector::TemplateInfo> TemplateMatchCorrector::buildTemplates(
    const cv::Mat& templateEdge) const
{
    std::vector<TemplateInfo> templates;
    for (double scale = _minScale; scale <= _maxScale + 1e-6;
        scale += (_scaleStep > 0.0 ? _scaleStep : 1.0)) {
        for (double angle = _minAngle; angle <= _maxAngle + 1e-6; angle += _angleStep) {
            TemplateInfo info;
            info.edgeImage = rotateNoCrop(templateEdge, angle, scale);
            info.angle = angle;
            info.scale = scale;
            info.originalTemplateSize = templateEdge.size();
            templates.push_back(info);
        }
        if (_scaleStep <= 0.0) {
            break;
        }
    }
    return templates;
}

std::vector<TemplateMatchCorrector::MatchCandidate> TemplateMatchCorrector::matchAllTemplates(
    const cv::Mat& searchEdge, const std::vector<TemplateInfo>& templates) const
{
    std::vector<MatchCandidate> candidates;
    // Global search at 1/4 of the existing search resolution; refine the best
    // hypotheses at the original resolution and with the original score.
    // Keep every configured angle and scale. Low-confidence/small-template
    // cases fall back to the exhaustive search below.
    if (_fastSearch && !templates.empty()) {
        constexpr double coarseScale = 0.25;
        bool suitable = true;
        for (const auto& t : templates)
            if (t.edgeImage.cols < 32 || t.edgeImage.rows < 32)
                suitable = false;
        if (suitable) {
            struct Hypothesis {
                double score;
                size_t index;
                cv::Point point;
            };
            std::vector<Hypothesis> hypotheses;
            cv::Mat coarseSearch;
            cv::resize(searchEdge, coarseSearch, cv::Size(), coarseScale, coarseScale, cv::INTER_AREA);
            for (size_t i = 0; i < templates.size(); ++i) {
                cv::Mat t, response;
                cv::resize(templates[i].edgeImage, t, cv::Size(), coarseScale, coarseScale, cv::INTER_AREA);
                if (t.cols > coarseSearch.cols || t.rows > coarseSearch.rows)
                    continue;
                cv::matchTemplate(coarseSearch, t, response, cv::TM_CCOEFF_NORMED);
                // Multiple locations avoid losing the target in repeated textures.
                for (int peak = 0; peak < 2; ++peak) {
                    double score;
                    cv::Point point;
                    cv::minMaxLoc(response, nullptr, &score, nullptr, &point);
                    hypotheses.push_back({ score, i, cv::Point(point.x * 4, point.y * 4) });
                    cv::Rect suppress(point.x - 3, point.y - 3, 7, 7);
                    suppress &= cv::Rect(0, 0, response.cols, response.rows);
                    response(suppress).setTo(-1);
                }
            }
            std::sort(hypotheses.begin(), hypotheses.end(),
                [](const Hypothesis& a, const Hypothesis& b) { return a.score > b.score; });
            if (hypotheses.size() > 8)
                hypotheses.resize(8);
            for (const auto& h : hypotheses) {
                const auto& t = templates[h.index];
                cv::Rect roi(h.point.x - 8, h.point.y - 8, t.edgeImage.cols + 16, t.edgeImage.rows + 16);
                roi &= cv::Rect(0, 0, searchEdge.cols, searchEdge.rows);
                if (roi.width < t.edgeImage.cols || roi.height < t.edgeImage.rows)
                    continue;
                cv::Mat response;
                cv::matchTemplate(searchEdge(roi), t.edgeImage, response, cv::TM_CCOEFF_NORMED);
                double score;
                cv::Point point;
                cv::minMaxLoc(response, nullptr, &score, nullptr, &point);
                if (score < std::max(0.45, _scoreThreshold))
                    continue;
                MatchCandidate c;
                c.score = score;
                c.angle = t.angle;
                c.scale = t.scale;
                c.rect = cv::RotatedRect(cv::Point2f(roi.x + point.x + t.edgeImage.cols / 2.f,
                                             roi.y + point.y + t.edgeImage.rows / 2.f),
                    cv::Size2f(
                        t.originalTemplateSize.width * t.scale, t.originalTemplateSize.height * t.scale),
                    t.angle);
                candidates.push_back(c);
            }
            if (!candidates.empty())
                return candidates;
        }
    }
    for (const auto& tmpl : templates) {
        if (tmpl.edgeImage.cols > searchEdge.cols || tmpl.edgeImage.rows > searchEdge.rows) {
            continue;
        }

        cv::Mat result;
        // TM_CCOEFF_NORMED的结果矩阵不能再调用normalize(),否则每个模板自己的
        // 最佳匹配都会被强行拉到1.0,导致不同角度/尺度模板之间的分数失去可比性。
        cv::matchTemplate(searchEdge, tmpl.edgeImage, result, cv::TM_CCOEFF_NORMED);

        double maxScore = 0.0;
        cv::Point maxLoc;
        cv::minMaxLoc(result, nullptr, &maxScore, nullptr, &maxLoc);

        if (maxScore < _scoreThreshold) {
            continue;
        }

        MatchCandidate candidate;
        candidate.score = maxScore;
        candidate.angle = tmpl.angle;
        candidate.scale = tmpl.scale;
        cv::Point2f center(maxLoc.x + tmpl.edgeImage.cols / 2.0f, maxLoc.y + tmpl.edgeImage.rows / 2.0f);
        candidate.rect = cv::RotatedRect(center,
            cv::Size2f(tmpl.originalTemplateSize.width * static_cast<float>(tmpl.scale),
                tmpl.originalTemplateSize.height * static_cast<float>(tmpl.scale)),
            static_cast<float>(tmpl.angle));
        candidates.push_back(candidate);
    }
    return candidates;
}

double TemplateMatchCorrector::rotatedRectIou(const cv::RotatedRect& a, const cv::RotatedRect& b)
{
    std::vector<cv::Point2f> intersectionPoints;
    int result = cv::rotatedRectangleIntersection(a, b, intersectionPoints);
    if (result == cv::INTERSECT_NONE || intersectionPoints.empty()) {
        return 0.0;
    }

    std::vector<cv::Point2f> hull;
    cv::convexHull(intersectionPoints, hull);
    double intersectionArea = cv::contourArea(hull);
    double unionArea = a.size.area() + b.size.area() - intersectionArea;
    if (unionArea <= 0.0) {
        return 0.0;
    }
    return intersectionArea / unionArea;
}

std::vector<TemplateMatchCorrector::MatchCandidate> TemplateMatchCorrector::nonMaxSuppression(
    std::vector<MatchCandidate> candidates) const
{
    std::sort(candidates.begin(), candidates.end(),
        [](const MatchCandidate& lhs, const MatchCandidate& rhs) { return lhs.score > rhs.score; });

    std::vector<MatchCandidate> kept;
    std::vector<bool> removed(candidates.size(), false);
    for (size_t i = 0; i < candidates.size(); ++i) {
        if (removed[i]) {
            continue;
        }
        kept.push_back(candidates[i]);
        for (size_t j = i + 1; j < candidates.size(); ++j) {
            if (removed[j]) {
                continue;
            }
            if (rotatedRectIou(candidates[i].rect, candidates[j].rect) > _iouThreshold) {
                removed[j] = true;
            }
        }
    }
    return kept;
}

CorrectionResult TemplateMatchCorrector::correct(
    const cv::Mat& referenceImage, const cv::Mat& liveImage, const QRectF& referenceRect) const
{
    CorrectionResult result;

    if (referenceImage.empty() || liveImage.empty()) {
        result.message = "输入图像为空";
        return result;
    }

    if (referenceRect.isEmpty()) {
        result.message = "未设置基准区域,请先在\"基准图管理\"中框选一块模板区域";
        return result;
    }

    const cv::Rect fullRefRect(0, 0, referenceImage.cols, referenceImage.rows);
    cv::Rect templateRect
        = cv::Rect(static_cast<int>(referenceRect.x()), static_cast<int>(referenceRect.y()),
              static_cast<int>(referenceRect.width()), static_cast<int>(referenceRect.height()))
        & fullRefRect;
    if (templateRect.width < 8 || templateRect.height < 8) {
        result.message = "基准区域过小或超出基准图范围,请重新框选";
        return result;
    }

    cv::Mat templateCrop = referenceImage(templateRect).clone();
    // 做高斯模糊 做canny边缘提取
    cv::Mat templateEdgeFull = toEdge(templateCrop);
    cv::Mat liveEdgeFull = toEdge(liveImage);

    // 金字塔降采样:先在缩小图上搜索,大幅减少批量模板匹配的计算量,
    // 匹配到位置后再按比例还原到原图坐标系。
    double pyramidScale = 1.0;
    for (int i = 0; i < _pyramidLevel; ++i) {
        pyramidScale *= 0.5;
    }

    cv::Mat templateEdge = templateEdgeFull;
    cv::Mat liveEdge = liveEdgeFull;
    if (_pyramidLevel > 0) {
        cv::resize(templateEdgeFull, templateEdge, cv::Size(), pyramidScale, pyramidScale, cv::INTER_LINEAR);
        cv::resize(liveEdgeFull, liveEdge, cv::Size(), pyramidScale, pyramidScale, cv::INTER_LINEAR);
    }

    if (templateEdge.cols < 4 || templateEdge.rows < 4) {
        result.message = "基准区域降采样后过小,请调大框选区域或降低金字塔层级";
        return result;
    }

    const std::vector<double> parameters { _minAngle, _maxAngle, _angleStep, _minScale, _maxScale, _scaleStep,
        double(_pyramidLevel) };
    const bool sameCrop = _cachedCrop.size() == templateCrop.size()
        && _cachedCrop.type() == templateCrop.type() && !_cachedCrop.empty()
        && cv::norm(_cachedCrop, templateCrop, cv::NORM_INF) == 0;
    if (!sameCrop || parameters != _cachedParameters) {
        _cachedTemplates = buildTemplates(templateEdge);
        _cachedCrop = templateCrop;
        _cachedParameters = parameters;
    }
    const auto& templates = _cachedTemplates;
    if (templates.empty()) {
        result.message = "未生成任何模板,检查角度/尺度范围配置";
        return result;
    }

    std::vector<MatchCandidate> candidates = matchAllTemplates(liveEdge, templates);
    if (candidates.empty()) {
        result.message = QString("未找到分数超过阈值(%1)的匹配,建议降低阈值或重新框选特征更明显的区域")
                             .arg(_scoreThreshold);
        return result;
    }

    std::vector<MatchCandidate> best = nonMaxSuppression(std::move(candidates));
    const MatchCandidate& top = best.front();

    // 匹配结果的中心坐标是在降采样图上取得的,按金字塔比例还原到原图坐标系
    double restoreScale = _pyramidLevel > 0 ? 1.0 / pyramidScale : 1.0;
    cv::Point2f centerInLive(top.rect.center.x * static_cast<float>(restoreScale),
        top.rect.center.y * static_cast<float>(restoreScale));
    // 模板区域在基准图中的实际中心(用户手动框选的referenceRect中心),
    // 而不是整幅基准图的中心 —— 这样无论用户框在图像哪个位置,对齐锚点都正确。
    cv::Point2f centerInRef(
        templateRect.x + templateRect.width / 2.0f, templateRect.y + templateRect.height / 2.0f);

    // 由匹配到的旋转矩形反推:live图中该模板区域相对基准图中该区域的平移、
    // 旋转角、缩放,构造把live图对齐到reference坐标系的仿射矩阵。
    // top.angle是"模板旋转top.angle度后与live匹配"得到的角度,即
    // live局部区域 ≈ rotate(模板, top.angle),所以要把live转回reference需要
    // 旋转-top.angle度。注意cv::getRotationMatrix2D采用图像坐标系(y轴向下)的
    // 旋转矩阵约定 R = [[cos,sin],[-sin,cos]],和标准数学旋转矩阵符号相反,
    // 这里必须保持一致,否则角度数值对但对齐方向会反。
    double angleRad = -top.angle * CV_PI / 180.0;
    double cosA = std::cos(angleRad);
    double sinA = std::sin(angleRad);
    double invScale = top.scale > 1e-6 ? 1.0 / top.scale : 1.0;

    cv::Mat affine(2, 3, CV_64F);
    affine.at<double>(0, 0) = invScale * cosA;
    affine.at<double>(0, 1) = invScale * sinA;
    affine.at<double>(1, 0) = -invScale * sinA;
    affine.at<double>(1, 1) = invScale * cosA;

    double tx = centerInRef.x
        - (affine.at<double>(0, 0) * centerInLive.x + affine.at<double>(0, 1) * centerInLive.y);
    double ty = centerInRef.y
        - (affine.at<double>(1, 0) * centerInLive.x + affine.at<double>(1, 1) * centerInLive.y);
    affine.at<double>(0, 2) = tx;
    affine.at<double>(1, 2) = ty;

    cv::Mat corrected;
    cv::warpAffine(liveImage, corrected, affine, referenceImage.size());

    result.success = true;
    result.correctedImage = corrected;
    result.affineMatrix = affine;
    result.dx = tx;
    result.dy = ty;
    result.angle = top.angle;
    result.scale = top.scale;
    result.matchScore = top.score;
    result.message = QString("模板匹配成功,得分%1,角度%2°,缩放%3")
                         .arg(top.score, 0, 'f', 3)
                         .arg(top.angle, 0, 'f', 1)
                         .arg(top.scale, 0, 'f', 3);

    return result;
}
