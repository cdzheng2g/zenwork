#include "NumericParameterField.h"
#include "ui/theme/AppTheme.h"
#include <QSlider>
#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <QSignalBlocker>
#include <cmath>
NumericParameterField::NumericParameterField(const ParameterSpec& p, QWidget* parent)
    : QWidget(parent)
{
    setStyleSheet(AppTheme::controls());
    auto* row = new QHBoxLayout(this);
    row->setContentsMargins(0, 0, 0, 0);
    row->setSpacing(12);
    _slider = new QSlider(Qt::Horizontal, this);
    _slider->setMinimumHeight(30);
    _slider->setVisible(p.slider);
    _spin = new QDoubleSpinBox(this);
    _spin->setDecimals(p.decimals);
    _spin->setRange(p.minimum, p.maximum);
    _spin->setSingleStep(p.step);
    _spin->setSuffix(p.unit);
    _spin->setMinimumHeight(AppTheme::ControlHeight);
    _spin->setMinimumWidth(120);
    _spin->setKeyboardTracking(false);
    const int ticks = qMax(1, int(qMin(10000.0, std::ceil((p.maximum - p.minimum) / p.step))));
    _slider->setRange(0, ticks);
    connect(_slider, &QSlider::valueChanged, this, [this, p, ticks](int tick) {
        const double value = p.minimum + (p.maximum - p.minimum) * tick / ticks;
        _spin->setValue(
            qBound(p.minimum, p.minimum + std::round((value - p.minimum) / p.step) * p.step, p.maximum));
    });
    connect(_spin, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this, p, ticks](double value) {
        const QSignalBlocker block(_slider);
        _slider->setValue(
            p.maximum > p.minimum ? qRound((value - p.minimum) * ticks / (p.maximum - p.minimum)) : 0);
        emit valueChanged(value);
    });
    row->addWidget(_slider, 1);
    row->addWidget(_spin, p.slider ? 0 : 1);
    _spin->setValue(p.defaultValue.toDouble());
    const QSignalBlocker initialBlock(_slider);
    _slider->setValue(
        p.maximum > p.minimum ? qRound((_spin->value() - p.minimum) * ticks / (p.maximum - p.minimum)) : 0);
}
double NumericParameterField::value() const
{
    return _spin->value();
}
void NumericParameterField::setValue(double value)
{
    _spin->setValue(value);
}
