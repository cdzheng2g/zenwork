#pragma once
#include "ui/operators/OperatorEditor.h"
#include "ui/ImageCanvasWidget.h"
#include "infrastructure/reference/ReferenceImageManager.h"
#include <QVBoxLayout>
class TemplateOperatorPanel final : public OperatorEditor {
public:
    explicit TemplateOperatorPanel(QWidget* parent = nullptr)
        : OperatorEditor(parent)
    {
        auto* layout = new QVBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);
        _preview = new ImageCanvasWidget(this);
        layout->addWidget(_preview);
    }
    QJsonObject configuration() const override
    {
        return _stored;
    }
    void setConfiguration(const QJsonObject& p) override
    {
        _stored = p;
        refreshReferencePreview();
    }
    void refreshReferencePreview() override
    {
        const auto* ref = ReferenceImageManager::instance().currentReferenceImage();
        if (!ref || ref->image.empty()) {
            _preview->clearImage();
            return;
        }
        _preview->setImage(ref->image);
        _preview->setOverlayRect(ref->referenceRect);
    }

private:
    ImageCanvasWidget* _preview;
    QJsonObject _stored;
};
