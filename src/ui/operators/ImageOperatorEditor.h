#pragma once
#include "OperatorEditor.h"
#include "application/operators/OperatorRegistry.h"
#include <QPointF>
class ImageCanvasWidget;
class ParameterForm;

class ImageOperatorEditor final : public OperatorEditor {
public:
    explicit ImageOperatorEditor(const OperatorDefinition& definition, QWidget* parent = nullptr);
    QJsonObject configuration() const override;
    void setConfiguration(const QJsonObject& values) override;
    void refreshReferencePreview() override;
private:
    OperatorDefinition _definition;
    ParameterSchema _ordinaryParameters, _advancedParameters;
    ParameterForm* _ordinary;
    ParameterForm* _advanced;
    ImageCanvasWidget* _preview;
    QJsonObject _stored;
    QPointF _first;
    bool _hasFirst { false };
};
