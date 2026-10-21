#include "modules/operators/template_match/TemplateMatchCorrector.h"
#include <QCoreApplication>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QElapsedTimer>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <cstdio>
int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    if (argc != 3)
        return 2;
    cv::Mat ref = cv::imread(argv[1]);
    QFile f(argv[2]);
    if (ref.empty() || !f.open(QIODevice::ReadOnly))
        return 2;
    auto meta = QJsonDocument::fromJson(f.readAll()).object();
    auto v = meta.value("referenceRect").toObject();
    QRectF rect(v["x"].toDouble(), v["y"].toDouble(), v["width"].toDouble(), v["height"].toDouble());
    if (rect.isEmpty())
        return 3;
    TemplateMatchCorrector baseline, fast;
    baseline.setFastSearch(false);
    int failures = 0;
    for (double angle : { 0., 8., -18., 38. }) {
        cv::Mat matrix = cv::getRotationMatrix2D(cv::Point2f(ref.cols / 2.f, ref.rows / 2.f), angle, 1);
        matrix.at<double>(0, 2) += 24;
        matrix.at<double>(1, 2) -= 18;
        cv::Mat frame;
        cv::warpAffine(ref, frame, matrix, ref.size());
        QElapsedTimer timer;
        timer.start();
        auto a = baseline.correct(ref, frame, rect);
        auto oldMs = timer.elapsed();
        timer.restart();
        auto b = fast.correct(ref, frame, rect);
        auto newMs = timer.elapsed();
        bool same = a.success == b.success
            && (!a.success
                || (std::abs(a.angle - b.angle) <= 2 && std::abs(a.dx - b.dx) < 8
                    && std::abs(a.dy - b.dy) < 8));
        printf("%s rotation %.0f baseline %lldms fast %lldms score %.3f/%.3f angle %.1f/%.1f dx %.1f/%.1f dy "
               "%.1f/%.1f\n",
            same ? "PASS" : "FAIL", angle, oldMs, newMs, a.matchScore, b.matchScore, a.angle, b.angle, a.dx,
            b.dx, a.dy, b.dy);
        if (!same)
            ++failures;
    }
    cv::Mat blank(ref.size(), ref.type(), cv::Scalar::all(255));
    auto negative = fast.correct(ref, blank, rect);
    printf("%s blank remains NG\n", !negative.success ? "PASS" : "FAIL");
    if (negative.success)
        ++failures;
    return failures ? 1 : 0;
}
