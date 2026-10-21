#include "OperatorGeometry.h"

QJsonArray operatorImagePolygon(const QRectF& rect, const cv::Mat& toImage)
{
    QJsonArray points;
    for (const QPointF& point : { rect.topLeft(), rect.topRight(), rect.bottomRight(), rect.bottomLeft() }) {
        double x = point.x();
        double y = point.y();
        if (!toImage.empty()) {
            x = toImage.at<double>(0, 0) * point.x() + toImage.at<double>(0, 1) * point.y()
                + toImage.at<double>(0, 2);
            y = toImage.at<double>(1, 0) * point.x() + toImage.at<double>(1, 1) * point.y()
                + toImage.at<double>(1, 2);
        }
        points.append(QJsonArray { x, y });
    }
    return points;
}
