#include "ParameterForm.h"
#include "NumericParameterField.h"
#include "ToggleField.h"
#include "FileParameterField.h"
#include "ui/theme/AppTheme.h"
#include <QComboBox>
#include <QLineEdit>
#include <QFormLayout>
#include <QSignalBlocker>
ParameterForm::ParameterForm(const ParameterSchema& schema, QWidget* parent)
    : QWidget(parent)
    , _schema(schema)
{
    setStyleSheet(AppTheme::controls());
    auto* layout = new QFormLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setHorizontalSpacing(16);
    layout->setVerticalSpacing(AppTheme::FormSpacing);
    layout->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    layout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    for (const auto& p : schema) {
        QWidget* field = nullptr;
        if (p.kind == ParameterKind::Number) {
            auto* input = new NumericParameterField(p, this);
            connect(input, &NumericParameterField::valueChanged, this, &ParameterForm::changed);
            field = input;
        } else if (p.kind == ParameterKind::Boolean) {
            auto* input = new ToggleField(this);
            input->setAccessibleName(p.label);
            connect(input, &QAbstractButton::toggled, this, &ParameterForm::changed);
            field = input;
        } else if (p.kind == ParameterKind::Choice) {
            auto* input = new QComboBox(this);
            for (const auto& choice : p.choices)
                input->addItem(p.choiceLabels.value(choice, choice), choice);
            connect(input, qOverload<int>(&QComboBox::currentIndexChanged), this, &ParameterForm::changed);
            field = input;
        } else if (p.kind == ParameterKind::File) {
            auto* input = new FileParameterField(p.fileFilter, this);
            connect(input, &FileParameterField::changed, this, &ParameterForm::changed);
            field = input;
        } else {
            auto* input = new QLineEdit(this);
            connect(input, &QLineEdit::textEdited, this, &ParameterForm::changed);
            field = input;
        }
        if (p.kind != ParameterKind::Boolean)
            field->setMinimumHeight(AppTheme::ControlHeight);
        field->setObjectName(p.key);
        _fields.insert(p.key, field);
        layout->addRow(p.label, field);
    }
    setConfiguration({ });
}
QJsonObject ParameterForm::configuration() const
{
    auto values = _stored;
    for (const auto& p : _schema) {
        auto* w = _fields.value(p.key);
        if (p.kind == ParameterKind::Number)
            values[p.key] = static_cast<NumericParameterField*>(w)->value();
        else if (p.kind == ParameterKind::Boolean)
            values[p.key] = static_cast<ToggleField*>(w)->isChecked();
        else if (p.kind == ParameterKind::Choice)
            values[p.key] = static_cast<QComboBox*>(w)->currentData().toString();
        else if (p.kind == ParameterKind::File)
            values[p.key] = static_cast<FileParameterField*>(w)->value();
        else
            values[p.key] = static_cast<QLineEdit*>(w)->text();
    }
    return values;
}
void ParameterForm::setConfiguration(const QJsonObject& values)
{
    const QSignalBlocker quiet(this);
    _stored = parameterDefaults(_schema, values);
    for (const auto& p : _schema) {
        auto* w = _fields.value(p.key);
        const QSignalBlocker block(w);
        const auto v = _stored[p.key];
        if (p.kind == ParameterKind::Number)
            static_cast<NumericParameterField*>(w)->setValue(v.toDouble());
        else if (p.kind == ParameterKind::Boolean)
            static_cast<ToggleField*>(w)->setChecked(v.toBool());
        else if (p.kind == ParameterKind::Choice)
            static_cast<QComboBox*>(w)->setCurrentIndex(static_cast<QComboBox*>(w)->findData(v.toString()));
        else if (p.kind == ParameterKind::File)
            static_cast<FileParameterField*>(w)->setValue(v.toString());
        else
            static_cast<QLineEdit*>(w)->setText(v.toString());
    }
}
