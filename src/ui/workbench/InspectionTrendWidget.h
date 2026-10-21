#pragma once
#include <QWidget>
#include <QPainter>
#include <QPainterPath>
#include <QJsonObject>
#include <QJsonArray>
#include <QVector>
#include <algorithm>

class InspectionTrendWidget : public QWidget {
public:
    explicit InspectionTrendWidget(QWidget* parent = nullptr)
        : QWidget(parent)
    {
        setMinimumHeight(250);
    }
    void append(const QJsonObject& result)
    {
        _samples.append(result);
        if (_samples.size() > 40)
            _samples.removeFirst();
        update();
    }
    void clear()
    {
        _samples.clear();
        update();
    }
    void selectTool(const QString& id)
    {
        _toolId = id;
        update();
    }

protected:
    void paintEvent(QPaintEvent*) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        p.fillRect(rect(), QColor("#f8fafc"));
        p.setPen(QColor("#526174"));
        p.drawText(QRect(7, 2, width() - 14, 22), Qt::AlignLeft | Qt::AlignVCenter,
            QStringLiteral("耗时趋势 · 最近40次"));
        if (_samples.isEmpty()) {
            p.drawText(rect(), Qt::AlignCenter, QStringLiteral("等待检测"));
            return;
        }
        QVector<double> times;
        QStringList states;
        for (const auto& sample : _samples) {
            QJsonObject selected = sample;
            if (!_toolId.isEmpty()) {
                selected = { };
                for (const auto& value : sample.value("tools").toArray())
                    if (value.toObject().value("id").toString() == _toolId)
                        selected = value.toObject();
            }
            times.append(selected.isEmpty() ? -1 : selected.value("elapsedMs").toDouble());
            states.append(selected.value("status").toString());
        }
        const double maximum = std::max(1.0, *std::max_element(times.begin(), times.end()) * 1.15);
        const QRectF area(38, 48, width() - 48, height() - 155);
        p.setPen(QPen(QColor("#dbe3ec"), 1));
        for (int i = 0; i < 3; ++i)
            p.drawLine(QPointF(area.left(), area.top() + i * area.height() / 2),
                QPointF(area.right(), area.top() + i * area.height() / 2));
        p.setPen(QColor("#526174"));
        p.drawText(QRect(7, 25, width() - 14, 20), QStringLiteral("0–%1 ms").arg(maximum, 0, 'f', 0));
        const int sampleSlots = qMax(19, times.size() - 1);
        auto xAt = [&](int i) { return area.left() + i * area.width() / sampleSlots; };
        QPainterPath path;
        bool connected = false;
        for (int i = 0; i < times.size(); ++i) {
            if (times[i] < 0) {
                connected = false;
                continue;
            }
            const QPointF point(xAt(i), area.bottom() - times[i] / maximum * area.height());
            if (!connected)
                path.moveTo(point);
            else
                path.lineTo(point);
            connected = true;
        }
        p.setPen(QPen(QColor("#2563eb"), 2));
        p.drawPath(path);
        const QStringList labels { QStringLiteral("OK"), QStringLiteral("NG"), QStringLiteral("异常") };
        const QList<QColor> colors { QColor("#15803d"), QColor("#dc2626"), QColor("#d97706") };
        const double rowsTop = area.bottom() + 22;
        for (int row = 0; row < 3; ++row) {
            const double y = rowsTop + row * 24;
            p.setPen(colors[row]);
            p.drawText(QRectF(3, y - 10, 33, 20), Qt::AlignVCenter, labels[row]);
            p.setPen(QPen(QColor("#e2e8f0"), 1));
            p.drawLine(QPointF(area.left(), y), QPointF(area.right(), y));
        }
        for (int i = 0; i < states.size(); ++i) {
            if (states[i].isEmpty())
                continue;
            const int row = states[i] == "OK" ? 0 : states[i] == "NG" ? 1 : 2;
            p.setPen(Qt::NoPen);
            p.setBrush(colors[row]);
            p.drawEllipse(QPointF(xAt(i), rowsTop + row * 24), 2.2, 4);
        }
        p.setPen(QColor("#526174"));
        p.drawText(QRect(7, height() - 24, width() - 14, 22),
            times.last() < 0 ? QStringLiteral("本次未执行所选工具")
                             : QStringLiteral("最新 %1 ms").arg(times.last(), 0, 'f', 0));
    }

private:
    QVector<QJsonObject> _samples;
    QString _toolId;
};
