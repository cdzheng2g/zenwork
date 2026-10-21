#include "ImageCanvasWidget.h"

#include <opencv2/imgproc.hpp>

#include <QMouseEvent>
#include <QPainter>

ImageCanvasWidget::ImageCanvasWidget(QWidget* parent)
    : QWidget(parent)
{
    setMinimumSize(240, 180);
    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor(24, 24, 24));
    setPalette(pal);
}

void ImageCanvasWidget::setImage(const cv::Mat& image)
{
    if (image.empty()) {
        clearImage();
        return;
    }

    cv::Mat rgb;
    if (image.channels() == 1) {
        cv::cvtColor(image, rgb, cv::COLOR_GRAY2RGB);
    } else if (image.channels() == 3) {
        cv::cvtColor(image, rgb, cv::COLOR_BGR2RGB);
    } else {
        cv::cvtColor(image, rgb, cv::COLOR_BGRA2RGB);
    }

    QImage qimg(rgb.data, rgb.cols, rgb.rows, static_cast<int>(rgb.step), QImage::Format_RGB888);
    _pixmap = QPixmap::fromImage(qimg.copy());
    _imageSize = QSize(image.cols, image.rows);
    update();
}

void ImageCanvasWidget::clearImage()
{
    _pixmap = QPixmap();
    _imageSize = QSize();
    _overlayRect = QRectF();
    _annotations.clear();
    update();
}

void ImageCanvasWidget::setMarkingEnabled(bool enabled)
{
    _markingEnabled = enabled;
    setCursor(enabled ? Qt::CrossCursor : Qt::ArrowCursor);
}

void ImageCanvasWidget::setOverlayRect(const QRectF& rect)
{
    _overlayRect = rect;
    update();
}

QSize ImageCanvasWidget::sizeHint() const
{

    return _imageSize.isEmpty() ? QSize(480, 360) : _imageSize.scaled(QSize(640, 360), Qt::KeepAspectRatio);
}

void ImageCanvasWidget::setAnnotations(const QList<ImageAnnotation>& annotations)
{
    _annotations = annotations;
    update();
}

QRectF ImageCanvasWidget::imageDisplayRect() const
{
    if (_pixmap.isNull()) {
        return QRectF();
    }

    const QSizeF imgSize(_pixmap.size());
    const QSizeF fitted = imgSize.scaled(QSizeF(size()), Qt::KeepAspectRatio);
    const qreal x = (width() - fitted.width()) / 2.0;
    const qreal y = (height() - fitted.height()) / 2.0;
    return QRectF(x, y, fitted.width(), fitted.height());
}

QPointF ImageCanvasWidget::widgetToImage(const QPointF& widgetPoint) const
{
    const QRectF displayRect = imageDisplayRect();
    if (displayRect.isEmpty() || !displayRect.contains(widgetPoint)) {
        return QPointF(-1, -1);
    }

    const qreal scaleX = _pixmap.width() / displayRect.width();
    const qreal scaleY = _pixmap.height() / displayRect.height();
    return QPointF(
        (widgetPoint.x() - displayRect.x()) * scaleX, (widgetPoint.y() - displayRect.y()) * scaleY);
}

QPointF ImageCanvasWidget::imageToWidget(const QPointF& imagePoint) const
{
    const QRectF displayRect = imageDisplayRect();
    if (displayRect.isEmpty()) {
        return QPointF();
    }

    const qreal scaleX = displayRect.width() / _pixmap.width();
    const qreal scaleY = displayRect.height() / _pixmap.height();
    return QPointF(displayRect.x() + imagePoint.x() * scaleX, displayRect.y() + imagePoint.y() * scaleY);
}

void ImageCanvasWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    painter.fillRect(rect(), palette().color(QPalette::Window));

    if (_pixmap.isNull()) {
        painter.setPen(QColor(160, 160, 160));
        painter.drawText(rect(), Qt::AlignCenter, "无图像");
        return;
    }

    const QRectF displayRect = imageDisplayRect();
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.drawPixmap(displayRect, _pixmap, QRectF(_pixmap.rect()));

    if (!_overlayRect.isEmpty()) {
        const QPointF topLeft = imageToWidget(_overlayRect.topLeft());
        const QPointF bottomRight = imageToWidget(_overlayRect.bottomRight());
        painter.setPen(QPen(QColor(0, 220, 0), 2));
        painter.setBrush(Qt::NoBrush);
        painter.drawRect(QRectF(topLeft, bottomRight));
    }
    painter.setClipRect(displayRect);
    QList<QRectF> labels;
    for (const auto& annotation : _annotations) {
        QPolygonF polygon;
        for (const auto& point : annotation.polygon)
            polygon << imageToWidget(point);
        painter.setPen(QPen(annotation.color, 2));
        painter.setBrush(Qt::NoBrush);
        if (polygon.isEmpty())
            continue;
        painter.drawPolygon(polygon);
        const int maxWidth = qMax(1, qMin(220, int(displayRect.width()) - 8));
        const QString label
            = painter.fontMetrics().elidedText(annotation.label, Qt::ElideRight, maxWidth - 10);
        const qreal h = painter.fontMetrics().height() + 6;
        const qreal w = qMin(qreal(maxWidth), qreal(painter.fontMetrics().horizontalAdvance(label) + 10));
        const auto box = polygon.boundingRect();
        const qreal x = qBound(displayRect.left() + 3, box.left(), displayRect.right() - w - 3);
        const qreal initial = box.top() - h - 3 >= displayRect.top() ? box.top() - h - 3 : box.bottom() + 3;
        QRectF chosen;
        // Prefer the edge of the detection region, then find a free label slot.
        // If the image is crowded, omit the text; full output remains in the table.
        for (int i = 0; i < int(displayRect.height() / h) + 2; ++i) {
            const qreal y = i == 0 ? initial : displayRect.top() + 3 + (i - 1) * (h + 2);
            const QRectF candidate(x, y, w, h);
            if (!displayRect.contains(candidate))
                continue;
            bool overlap = false;
            for (const auto& prior : labels)
                if (prior.adjusted(-2, -2, 2, 2).intersects(candidate)) {
                    overlap = true;
                    break;
                }
            if (!overlap) {
                chosen = candidate;
                break;
            }
        }
        if (chosen.isEmpty())
            continue;
        labels.append(chosen);
        painter.fillRect(chosen, QColor(15, 23, 42, 215));
        painter.setPen(annotation.color);
        painter.drawText(chosen.adjusted(5, 0, -3, 0), Qt::AlignVCenter, label);
    }
}

void ImageCanvasWidget::mousePressEvent(QMouseEvent* event)
{
    if (_markingEnabled && event->button() == Qt::LeftButton && !_pixmap.isNull()) {
        const QPointF imagePoint = widgetToImage(QPointF(event->pos()));
        if (imagePoint.x() >= 0 && imagePoint.y() >= 0) {
            Q_EMIT canvasPointClicked(imagePoint);
        }
    }
    QWidget::mousePressEvent(event);
}

void ImageCanvasWidget::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    update();
}
