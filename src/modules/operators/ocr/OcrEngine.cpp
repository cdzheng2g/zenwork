#include "OcrEngine.h"
#include "OcrModule.h"
#include "application/operators/OperatorResources.h"
#include "application/vision/ImageTransform.h"
#include "infrastructure/inference/rknn/RknnSession.h"
#include <QFile>
#include <QJsonArray>
#include <QStringList>
#include <opencv2/imgproc.hpp>
#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace {
void require(bool ok, const QString& text)
{
    if (!ok)
        throw std::runtime_error(text.toStdString());
}
QString modelPath(const QJsonObject& config, const char* key)
{
    for (const auto& spec : ocrDefinition().resources)
        if (spec.key == key)
            return operatorResourcePath(config, spec);
    throw std::runtime_error("OCR 资源未声明");
}
class OcrImageModel {
    RknnSession session;

public:
    int width { 0 }, height { 0 };
    TensorInfo output;
    void load(const QString& path)
    {
        session.load(path);
        require(session.inputs().size() == 1 && session.outputs().size() == 1,
            "OCR 适配器需要单输入、单输出模型");
        const auto& input = session.inputs().front();
        require(input.shape.size() == 4 && input.shape[0] == 1 && input.layout != TensorLayout::Other,
            "OCR 模型需要固定尺寸的单张图像输入");
        const bool nchw = input.layout == TensorLayout::Nchw;
        height = int(input.shape[nchw ? 2 : 1]);
        width = int(input.shape[nchw ? 3 : 2]);
        require(input.shape[nchw ? 1 : 3] == 3 && width > 0 && height > 0, "OCR 模型需要三通道输入");
        output = session.outputs().front();
    }
    std::vector<float> infer(const cv::Mat& rgb)
    {
        require(rgb.type() == CV_8UC3 && rgb.cols == width && rgb.rows == height && rgb.isContinuous(),
            "OCR 输入格式错误");
        auto outputs = session.run({ {rgb.data, rgb.total() * rgb.elemSize(),
            TensorInputType::UInt8, TensorLayout::Nhwc} });
        return std::move(outputs.front().values);
    }
};
struct OcrEngine {
    OcrImageModel rec, det;
    QPair<QString, double> recognize(const cv::Mat& rgb, const QStringList& dictionary)
    {
        cv::Mat canvas(rec.height, rec.width, CV_8UC3, cv::Scalar(128, 128, 128)), scaled;
        const int w
            = std::max(1, std::min(rec.width, int(std::ceil(double(rgb.cols) * rec.height / rgb.rows))));
        cv::resize(rgb, scaled, { w, rec.height });
        scaled.copyTo(canvas(cv::Rect(0, 0, w, rec.height)));
        const auto values = rec.infer(canvas);
        const int classes = dictionary.size();
        require(classes > 1 && rec.output.shape.size() >= 2
                && rec.output.shape.back() == unsigned(classes)
                && values.size() % classes == 0,
            "识别模型与字符字典不匹配");
        QString text;
        double sum = 0;
        int count = 0, previous = -1;
        for (size_t t = 0; t < values.size() / classes; ++t) {
            auto begin = values.begin() + t * classes;
            int index = std::max_element(begin, begin + classes) - begin;
            if (index && index != previous) {
                text += dictionary[index];
                sum += begin[index];
                ++count;
            }
            previous = index;
        }
        return { text, count ? sum / count : 0 };
    }
    QJsonObject run(const cv::Mat& bgr, const QJsonObject& config)
    {
        rec.load(modelPath(config, "recognitionModel"));
        QFile f(modelPath(config, "dictionary"));
        require(f.open(QIODevice::ReadOnly), "无法读取 OCR 字典");
        QStringList dict { QString() };
        while (!f.atEnd()) {
            QString line = QString::fromUtf8(f.readLine());
            if (line.endsWith('\n'))
                line.chop(1);
            if (line.endsWith('\r'))
                line.chop(1);
            dict << line;
        }
        dict << " ";
        cv::Mat rgb;
        cv::cvtColor(bgr, rgb, cv::COLOR_BGR2RGB);
        std::vector<cv::Rect> boxes;
        if (config.value("fullImage").toBool(false)) {
            det.load(modelPath(config, "detectionModel"));
            const auto prepared = resizeForModel(rgb, {det.width, det.height}, ImageResizeMode::Stretch);
            auto values = det.infer(prepared.image);
            require(values.size() == size_t(det.width * det.height), "不支持的文字检测输出尺寸");
            cv::Mat probs(det.height, det.width, CV_32F, values.data()), mask;
            cv::compare(probs, .3, mask, cv::CMP_GT);
            std::vector<std::vector<cv::Point>> contours;
            cv::findContours(mask, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
            for (const auto& contour : contours) {
                cv::Rect r = cv::boundingRect(contour);
                if (r.area() < 12 || r.width < 3 || r.height < 3)
                    continue;
                cv::Mat region = cv::Mat::zeros(mask.size(), CV_8U);
                std::vector<std::vector<cv::Point>> one { contour };
                cv::drawContours(region, one, 0, cv::Scalar(255), cv::FILLED);
                if (cv::mean(probs, region)[0] < .5)
                    continue;
                int pad = std::max(2, r.height / 3);
                r.x -= pad;
                r.y -= pad;
                r.width += 2 * pad;
                r.height += 2 * pad;
                r &= cv::Rect(0, 0, det.width, det.height);
                const auto restored = prepared.sourceRect(QRectF(r.x, r.y, r.width, r.height));
                cv::Rect mapped(int(restored.x()), int(restored.y()),
                    std::max(1, int(restored.width())), std::max(1, int(restored.height())));
                mapped &= cv::Rect(0, 0, rgb.cols, rgb.rows);
                if (mapped.area() > 0)
                    boxes.push_back(mapped);
            }
            std::sort(boxes.begin(), boxes.end(),
                [](const cv::Rect& a, const cv::Rect& b) { return a.y == b.y ? a.x < b.x : a.y < b.y; });
            require(boxes.size() <= 64, "文字区域超过 64 个，请缩小检测范围");
        } else
            boxes.push_back({ 0, 0, rgb.cols, rgb.rows });
        QStringList texts;
        QJsonArray regions;
        double sum = 0;
        int count = 0;
        for (const auto& box : boxes) {
            auto best = recognize(rgb(box), dict);
            if (config.value("autoRotate").toBool(true)) {
                cv::Mat rotated;
                cv::rotate(rgb(box), rotated, cv::ROTATE_180);
                auto alternative = recognize(rotated, dict);
                if (alternative.second > best.second)
                    best = alternative;
            }
            if (best.first.isEmpty())
                continue;
            texts << best.first;
            sum += best.second;
            ++count;
            regions.append(QJsonObject { { "text", best.first }, { "confidence", best.second },
                { "x", box.x }, { "y", box.y }, { "width", box.width }, { "height", box.height } });
        }
        return { { "text", texts.join(" ") }, { "confidence", count ? sum / count : 0 },
            { "regions", regions } };
    }
};
}

QJsonObject runOcrEngine(const cv::Mat& bgr, const QJsonObject& config)
{
    thread_local OcrEngine engine;
    return engine.run(bgr, config);
}
