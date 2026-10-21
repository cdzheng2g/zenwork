#pragma once
#include "contracts/ParameterSpec.h"
#include <QWidget>
#include <QMap>
class ParameterForm final : public QWidget {
    Q_OBJECT
public:
    explicit ParameterForm(const ParameterSchema& schema, QWidget* parent = nullptr);
    QJsonObject configuration() const;
    void setConfiguration(const QJsonObject& values);
signals:
    void changed();

private:
    ParameterSchema _schema;
    QJsonObject _stored;
    QMap<QString, QWidget*> _fields;
};
