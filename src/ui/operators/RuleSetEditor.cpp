#include "RuleSetEditor.h"
#include "modules/rules/RuleRegistry.h"
#include "ui/components/ParameterForm.h"
#include "ui/components/ToggleField.h"
#include "ui/theme/AppTheme.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QFrame>
RuleSetEditor::RuleSetEditor(const QList<OutputField>& outputs, QWidget* parent)
    : QWidget(parent)
    , _outputs(outputs)
{
    setStyleSheet(AppTheme::controls());
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    auto* header = new QHBoxLayout;
    auto* types = new QComboBox(this);
    for (const auto& d : RuleRegistry::definitions()) {
        bool supported = false;
        for (const auto& f : outputs)
            if (f.kind == d.inputKind)
                supported = true;
        if (supported)
            types->addItem(d.name, d.type);
    }
    auto* add = new QPushButton("添加判断", this);
    add->setMinimumHeight(38);
    add->setEnabled(types->count() > 0);
    header->addWidget(types, 1);
    header->addWidget(add);
    root->addLayout(header);
    _layout = new QVBoxLayout;
    _layout->setSpacing(16);
    root->addLayout(_layout);
    root->addStretch();
    connect(add, &QPushButton::clicked, this, [this, types] {
        addRule({ { "type", types->currentData().toString() }, { "parameters", QJsonObject { } } });
    });
}
void RuleSetEditor::addRule(const QJsonObject& rule)
{
    const auto* d = RuleRegistry::find(rule["type"].toString());
    if (!d)
        return;
    auto* card = new QFrame(this);
    auto* layout = new QVBoxLayout(card);
    layout->setContentsMargins(0, 8, 0, 8);
    auto* header = new QHBoxLayout;
    header->addWidget(new QLabel(d->name, card));
    auto* fields = new QComboBox(card);
    for (const auto& f : _outputs)
        if (f.kind == d->inputKind)
            fields->addItem(f.label, f.key);
    const int index = fields->findData(rule["field"]);
    if (index >= 0)
        fields->setCurrentIndex(index);
    header->addWidget(fields, 1);
    auto* enabled = new ToggleField(card);
    enabled->setChecked(rule["enabled"].toBool(true));
    enabled->setAccessibleName("启用判断");
    header->addWidget(enabled);
    auto* remove = new QPushButton("移除", card);
    header->addWidget(remove);
    layout->addLayout(header);
    auto* form = new ParameterForm(d->parameters, card);
    form->setConfiguration(rule["parameters"].toObject());
    layout->addWidget(form);
    _rows.append({ card, d->type, fields, form, enabled });
    form->setEnabled(enabled->isChecked());
    fields->setEnabled(enabled->isChecked());
    connect(enabled, &QAbstractButton::toggled, form, &QWidget::setEnabled);
    connect(enabled, &QAbstractButton::toggled, fields, &QWidget::setEnabled);
    _layout->addWidget(card);
    connect(remove, &QPushButton::clicked, this, [this, card] {
        for (int i = 0; i < _rows.size(); ++i)
            if (_rows[i].widget == card) {
                _rows.removeAt(i);
                break;
            }
        _layout->removeWidget(card);
        card->deleteLater();
    });
}
QJsonArray RuleSetEditor::configuration() const
{
    QJsonArray result;
    for (const auto& row : _rows)
        result.append(QJsonObject { { "type", row.type }, { "field", row.field->currentData().toString() },
            { "parameters", row.form->configuration() }, { "enabled", row.enabled->isChecked() } });
    return result;
}
void RuleSetEditor::setConfiguration(const QJsonArray& rules)
{
    for (const auto& row : _rows)
        delete row.widget;
    _rows.clear();
    for (const auto& rule : rules)
        addRule(rule.toObject());
}
