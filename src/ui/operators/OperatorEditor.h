#pragma once
#include <QWidget>
#include <QJsonObject>
class OperatorEditor : public QWidget {
public:
    using QWidget::QWidget;
    virtual QJsonObject configuration() const = 0;
    virtual void setConfiguration(const QJsonObject& config) = 0;
    virtual void refreshReferencePreview() { }
};
