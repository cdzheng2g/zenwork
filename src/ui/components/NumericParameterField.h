#pragma once
#include "contracts/ParameterSpec.h"
#include <QWidget>
class QSlider;
class QDoubleSpinBox;
class NumericParameterField final : public QWidget {
    Q_OBJECT
public:
    explicit NumericParameterField(const ParameterSpec& spec, QWidget* parent = nullptr);
    double value() const;
    void setValue(double value);
    QSlider* slider() const
    {
        return _slider;
    }
    QDoubleSpinBox* spinBox() const
    {
        return _spin;
    }
signals:
    void valueChanged(double value);

private:
    QSlider* _slider;
    QDoubleSpinBox* _spin;
};
