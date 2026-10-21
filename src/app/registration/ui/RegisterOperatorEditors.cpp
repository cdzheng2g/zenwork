#include "ui/operators/OperatorEditorRegistry.h"
#include "ui/components/ParameterForm.h"
#include "application/operators/OperatorRegistry.h"
#include "ui/operators/ImageOperatorEditor.h"
#include "modules/operators/template_match/TemplateOperatorPanel.h"
#include <QVBoxLayout>
#include <QMap>
namespace {
class FormEditor final : public OperatorEditor {
public:
    FormEditor(const ParameterSchema& schema, QWidget* parent)
        : OperatorEditor(parent)
    {
        auto* layout = new QVBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);
        _form = new ParameterForm(schema, this);
        layout->addWidget(_form);
        layout->addStretch();
    }
    QJsonObject configuration() const override
    {
        return _form->configuration();
    }
    void setConfiguration(const QJsonObject& p) override
    {
        _form->setConfiguration(p);
    }

private:
    ParameterForm* _form;
};
}
OperatorEditor* OperatorEditorRegistry::create(const QString& kind, QWidget* parent)
{
    static const QMap<QString, OperatorEditorFactory> custom {
        { "template", [](QWidget* p) { return new TemplateOperatorPanel(p); } }
    };
    const auto* d = OperatorRegistry::find(kind);
    if (!d)
        return nullptr;
    const auto factory = custom.value(kind);
    if (factory)
        return factory(parent);
    if (d->imageInput)
        return new ImageOperatorEditor(*d, parent);
    return new FormEditor(d->parameters, parent);
}
