#include "ImageOperatorEditor.h"
#include "ui/components/ParameterForm.h"
#include "ui/ImageCanvasWidget.h"
#include "infrastructure/reference/ReferenceImageManager.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <stdexcept>

ImageOperatorEditor::ImageOperatorEditor(const OperatorDefinition& definition, QWidget* parent)
    : OperatorEditor(parent), _definition(definition)
{
    if (!_definition.imageInput)
        throw std::invalid_argument("ImageOperatorEditor requires imageInput metadata");
    auto* root = new QHBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(14);
    auto* imagePane = new QVBoxLayout;
    _preview = new ImageCanvasWidget(this);
    _preview->setObjectName("operatorImagePreview");
    _preview->setMinimumSize(300, 230);
    imagePane->addWidget(_preview, 1);
    auto* actions = new QHBoxLayout;
    auto* mark = new QPushButton("设置处理区域", this);
    mark->setObjectName("markOperatorRoi");
    mark->setCheckable(true);
    auto* reset = new QPushButton("使用基准区域", this);
    reset->setObjectName("resetOperatorRoi");
    if (!_definition.imageInput->fallbackToReference)
        reset->setText("清除处理区域");
    actions->addWidget(mark);
    actions->addWidget(reset);
    imagePane->addLayout(actions);
    root->addLayout(imagePane, 1);
    connect(mark, &QPushButton::toggled, this, [this, mark](bool enabled) {
        _hasFirst = false;
        _preview->setMarkingEnabled(enabled);
        mark->setText(enabled ? "依次点击区域两个角点" : "设置处理区域");
    });
    connect(reset, &QPushButton::clicked, this, [this, mark] {
        mark->setChecked(false);
        _stored.remove(_definition.imageInput->roiKey);
        refreshReferencePreview();
    });
    connect(_preview, &ImageCanvasWidget::canvasPointClicked, this, [this, mark](const QPointF& point) {
        if (!mark->isChecked())
            return;
        if (!_hasFirst) {
            _first = point;
            _hasFirst = true;
            return;
        }
        const auto roi = QRectF(_first, point).normalized();
        if (roi.width() < 1 || roi.height() < 1)
            return;
        _stored[_definition.imageInput->roiKey] = QJsonArray {roi.x(), roi.y(), roi.width(), roi.height()};
        if (!_definition.imageInput->fullImageKey.isEmpty()) {
            auto values = _ordinary->configuration();
            values[_definition.imageInput->fullImageKey] = false;
            _ordinary->setConfiguration(values);
        }
        mark->setChecked(false);
        refreshReferencePreview();
    });
    auto* config = new QVBoxLayout;
    for (const auto& parameter : _definition.parameters)
        (parameter.advanced ? _advancedParameters : _ordinaryParameters).append(parameter);
    _ordinary = new ParameterForm(_ordinaryParameters, this);
    _advanced = new ParameterForm(_advancedParameters, this);
    config->addWidget(_ordinary);
    connect(_ordinary, &ParameterForm::changed, this, [this] { refreshReferencePreview(); });
    auto* advanced = new QPushButton("高级设置", this);
    advanced->setCheckable(true);
    advanced->setVisible(!_advancedParameters.isEmpty());
    config->addWidget(advanced);
    _advanced->hide();
    connect(advanced, &QPushButton::toggled, _advanced, &QWidget::setVisible);
    config->addWidget(_advanced);
    config->addStretch();
    root->addLayout(config, 1);
    setConfiguration({});
}

QJsonObject ImageOperatorEditor::configuration() const
{
    auto values = _stored;
    const auto ordinary = _ordinary->configuration(), advanced = _advanced->configuration();
    for (const auto& parameter : _definition.parameters)
        values[parameter.key] = (parameter.advanced ? advanced : ordinary).value(parameter.key);
    return values;
}

void ImageOperatorEditor::setConfiguration(const QJsonObject& values)
{
    _stored = values;
    _hasFirst = false;
    if (auto* mark = findChild<QPushButton*>("markOperatorRoi"))
        mark->setChecked(false);
    _ordinary->setConfiguration(values);
    _advanced->setConfiguration(values);
    refreshReferencePreview();
}

void ImageOperatorEditor::refreshReferencePreview()
{
    const auto* ref = ReferenceImageManager::instance().currentReferenceImage();
    if (!ref || ref->image.empty()) {
        _preview->clearImage();
        return;
    }
    _preview->setImage(ref->image);
    const auto& spec = *_definition.imageInput;
    const auto values = configuration();
    const bool whole = spec.fullImageKey.isEmpty() ? spec.fullImageDefault
        : values.value(spec.fullImageKey).toBool(spec.fullImageDefault);
    const auto saved = values.value(spec.roiKey).toArray();
    const auto rect = saved.size() == 4
        ? QRectF(saved[0].toDouble(), saved[1].toDouble(), saved[2].toDouble(), saved[3].toDouble())
        : spec.fallbackToReference ? ref->referenceRect : QRectF();
    _preview->setOverlayRect(whole ? QRectF() : rect);
}
