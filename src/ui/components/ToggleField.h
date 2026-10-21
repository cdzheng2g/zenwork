#pragma once
#include <QAbstractButton>
#include <QPainter>
class ToggleField final : public QAbstractButton {
public:
    explicit ToggleField(QWidget* parent = nullptr)
        : QAbstractButton(parent)
    {
        setCheckable(true);
        setFixedSize(54, 30);
        setCursor(Qt::PointingHandCursor);
        setFocusPolicy(Qt::StrongFocus);
    }

protected:
    void paintEvent(QPaintEvent*) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        p.setPen(Qt::NoPen);
        p.setBrush(!isEnabled() ? QColor("#d6dbe1") : isChecked() ? QColor("#b51d32") : QColor("#c2c9d1"));
        p.drawRoundedRect(QRectF(1, 2, 52, 26), 13, 13);
        p.setBrush(Qt::white);
        p.drawEllipse(QRectF(isChecked() ? 29 : 5, 6, 18, 18));
        if (hasFocus()) {
            p.setBrush(Qt::NoBrush);
            p.setPen(QColor("#b51d32"));
            p.drawRoundedRect(QRectF(0, 0, 53, 29), 14, 14);
        }
    }
};
