#pragma once
#include "contracts/ParameterSpec.h"
#include <QWidget>
#include <QJsonArray>
class QVBoxLayout;
class QComboBox;
class ParameterForm;
class ToggleField;
class RuleSetEditor final : public QWidget {
public:
    explicit RuleSetEditor(const QList<OutputField>& outputs, QWidget* parent = nullptr);
    QJsonArray configuration() const;
    void setConfiguration(const QJsonArray& rules);

private:
    void addRule(const QJsonObject& rule);
    struct Row {
        QWidget* widget;
        QString type;
        QComboBox* field;
        ParameterForm* form;
        ToggleField* enabled;
    };
    QList<OutputField> _outputs;
    QList<Row> _rows;
    QVBoxLayout* _layout;
};
