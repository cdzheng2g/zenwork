#ifndef IMAGECANVASWIDGET_H
#define IMAGECANVASWIDGET_H

#include <opencv2/core.hpp>

#include <QPixmap>
#include <QWidget>
#include <QPolygonF>
#include <QColor>

struct ImageAnnotation {
    QPolygonF polygon;
    QString label;
    QColor color;
};

// 通用图像画布:显示cv::Mat图像(自动按控件尺寸等比缩放居中),支持
// 鼠标点击标记基准区域角点(以图像原始像素坐标记录/回调),并可叠加绘制矩形框
// (基准区域/校正结果可视化)。ElaWidgetTools没有现成的图像标注控件,
// 这里用QWidget+QPainter自行实现,后续基准图管理/校正结果两个页面复用它。
class ImageCanvasWidget : public QWidget {
    Q_OBJECT
public:
    explicit ImageCanvasWidget(QWidget* parent = nullptr);

    void setImage(const cv::Mat& image);
    void clearImage();

    // 框选角点标记(图像像素坐标系),点击画布时若_markingEnabled为true会通过
    // canvasPointClicked信号上报,由调用者决定如何使用(如构造基准区域矩形)。
    void setMarkingEnabled(bool enabled);

    // 叠加矩形框(图像像素坐标系),传空矩形则不显示
    void setOverlayRect(const QRectF& rect);
    void setAnnotations(const QList<ImageAnnotation>& annotations);

    QSize sizeHint() const override;

Q_SIGNALS:
    void canvasPointClicked(const QPointF& imagePoint);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    QRectF imageDisplayRect() const;
    QPointF widgetToImage(const QPointF& widgetPoint) const;
    QPointF imageToWidget(const QPointF& imagePoint) const;

    QPixmap _pixmap;
    QSize _imageSize;
    bool _markingEnabled { false };
    QRectF _overlayRect;
    QList<ImageAnnotation> _annotations;
};

#endif // IMAGECANVASWIDGET_H
