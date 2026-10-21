#include "ReferenceManagePage.h"
#include "../FileOperation.h"
#include <QFileSystemModel>
#include <QFileInfo>
#include <QTreeView>
#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QDir>
#include <QImageReader>
#include <QResizeEvent>
#include <memory>
#include <opencv2/imgcodecs.hpp>

#include "ImageCanvasWidget.h"
#include "infrastructure/reference/ReferenceImageManager.h"
#include "infrastructure/camera/CameraStreamService.h"

#include "ElaComboBox.h"
#include "ElaLineEdit.h"
#include "ElaMessageBar.h"
#include "ElaPushButton.h"
#include "ElaText.h"

#include <QFileDialog>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QVBoxLayout>

ReferenceManagePage::ReferenceManagePage(QWidget* parent)
    : QWidget(parent)
{
    initContent();
    refreshModelList();
}

void ReferenceManagePage::initContent()
{
    QHBoxLayout* rootLayout = new QHBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(14);

    // 左侧:操作面板
    QWidget* panel = new QWidget(this);
    panel->setFixedWidth(280);
    QVBoxLayout* panelLayout = new QVBoxLayout(panel);
    panelLayout->setContentsMargins(0, 0, 0, 0);
    panelLayout->setSpacing(12);

    ElaText* titleText = new ElaText("基准图管理", panel);
    titleText->setTextPixelSize(18);

    _modelComboBox = new ElaComboBox(panel);
    _modelComboBox->setFixedHeight(36);
    connect(_modelComboBox, &ElaComboBox::currentTextChanged, this, &ReferenceManagePage::onModelChanged);

    _modelNameEdit = new ElaLineEdit(panel);
    _modelNameEdit->setFixedHeight(36);
    _modelNameEdit->setPlaceholderText("新基准图型号名称");

    _importButton = new ElaPushButton("导入基准图...", panel);
    _importButton->setFixedHeight(36);
    connect(_importButton, &ElaPushButton::clicked, this, &ReferenceManagePage::onImportClicked);

    _captureLiveButton = new ElaPushButton("采集实时图为基准...", panel);
    _captureLiveButton->setFixedHeight(36);
    connect(
        _captureLiveButton, &ElaPushButton::clicked, this, &ReferenceManagePage::onCaptureLiveFrameClicked);

    ElaText* rectLabel = new ElaText("基准区域(模板匹配的模板来源,同时用于校正后叠加显示)", panel);
    rectLabel->setTextPixelSize(14);
    rectLabel->setIsWrapAnywhere(true);

    _markRectButton = new ElaPushButton("标记基准区域(点两角)", panel);
    _markRectButton->setFixedHeight(36);
    _markRectButton->setCheckable(true);
    connect(_markRectButton, &ElaPushButton::toggled, this, &ReferenceManagePage::onMarkRectToggled);

    _statusText = new ElaText("请先导入基准图", panel);
    _statusText->setTextPixelSize(12);
    _statusText->setIsWrapAnywhere(true);

    panelLayout->addWidget(titleText);
    panelLayout->addSpacing(4);
    panelLayout->addWidget(_modelComboBox);
    panelLayout->addWidget(_modelNameEdit);
    panelLayout->addWidget(_importButton);
    panelLayout->addWidget(_captureLiveButton);
    panelLayout->addSpacing(8);
    panelLayout->addWidget(rectLabel);
    panelLayout->addWidget(_markRectButton);
    panelLayout->addStretch(1);
    panelLayout->addWidget(_statusText);

    // 右侧:图像画布
    _canvas = new ImageCanvasWidget(this);
    _canvas->setMinimumSize(320, 240);
    connect(_canvas, &ImageCanvasWidget::canvasPointClicked, this, &ReferenceManagePage::onCanvasPointMarked);
    connect(&CameraStreamService::instance(), &CameraStreamService::frameReady, this,
        &ReferenceManagePage::onLiveFrameReady);

    rootLayout->addWidget(panel);
    rootLayout->addWidget(_canvas, 1);
}

void ReferenceManagePage::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    if (!_cameraLeaseHeld) {
        CameraStreamService::instance().acquire();
        _cameraLeaseHeld = true;
    }
}

void ReferenceManagePage::hideEvent(QHideEvent* event)
{
    if (_cameraLeaseHeld) {
        CameraStreamService::instance().release();
        _cameraLeaseHeld = false;
    }
    QWidget::hideEvent(event);
}

void ReferenceManagePage::refreshModelList()
{
    const QStringList models = ReferenceImageManager::instance().availableModels();
    _modelComboBox->blockSignals(true);
    _modelComboBox->clear();
    _modelComboBox->addItems(models);
    _modelComboBox->setCurrentIndex(-1);
    const QString current = ReferenceImageManager::instance().currentModel();
    if (!current.isEmpty()) {
        _modelComboBox->setCurrentText(current);
    }
    _modelComboBox->blockSignals(false);
    refreshCurrentModelView();
}

void ReferenceManagePage::refreshCurrentModelView()
{
    const ReferenceImage* refImage = ReferenceImageManager::instance().currentReferenceImage();
    if (!refImage) {
        _canvas->clearImage();
        _statusText->setText("请先导入基准图");
        return;
    }

    _canvas->setImage(refImage->image);
    _canvas->setOverlayRect(refImage->referenceRect);

    _statusText->setText(QString("型号: %1 | 基准区域: %2")
            .arg(refImage->modelName)
            .arg(refImage->referenceRect.isEmpty() ? "未设置" : "已设置"));
}

void ReferenceManagePage::onImportClicked()
{
    if (_filePanel || FileOperation::busy(this))
        return;
    const QString name = _modelNameEdit->text().trimmed();
    if (!ReferenceImageManager::isValidModelName(name)) {
        _statusText->setText(QStringLiteral("请先填写有效的型号名称"));
        return;
    }
    auto* panel = _filePanel = new QFrame(this);
    panel->setObjectName("configPanel");
    panel->setAutoFillBackground(true);
    auto* layout = new QVBoxLayout(panel);
    layout->addWidget(new QLabel(QStringLiteral("选择基准图 · %1").arg(name), panel));
    auto* model = new QFileSystemModel(panel);
    model->setRootPath(QDir::homePath());
    model->setNameFilters({ "*.png", "*.jpg", "*.jpeg", "*.bmp", "*.PNG", "*.JPG", "*.JPEG", "*.BMP" });
    model->setNameFilterDisables(false);
    auto* tree = new QTreeView(panel);
    tree->setModel(model);
    tree->setRootIndex(model->index("/"));
    tree->setCurrentIndex(model->index(QDir::homePath()));
    tree->expand(model->index(QDir::homePath()));
    tree->setColumnWidth(0, 400);
    tree->hideColumn(1);
    tree->hideColumn(2);
    tree->hideColumn(3);
    layout->addWidget(tree, 1);
    auto* path = new QLineEdit(panel);
    path->setPlaceholderText(QStringLiteral("选择图像或输入完整文件路径"));
    layout->addWidget(path);
    auto* feedback = new QLabel(QStringLiteral("支持 PNG、JPG、BMP，导入期间会显示处理状态。"), panel);
    feedback->setWordWrap(true);
    layout->addWidget(feedback);
    auto* actions = new QHBoxLayout();
    actions->addStretch();
    auto* cancel = new QPushButton(QStringLiteral("取消"), panel);
    auto* accept = new QPushButton(QStringLiteral("导入"), panel);
    actions->addWidget(cancel);
    actions->addWidget(accept);
    layout->addLayout(actions);
    connect(tree, &QTreeView::clicked, panel, [model, path](const QModelIndex& index) {
        if (!model->isDir(index))
            path->setText(model->filePath(index));
    });
    connect(cancel, &QPushButton::clicked, this, [this, panel] {
        _filePanel = nullptr;
        panel->hide();
        panel->deleteLater();
    });
    connect(accept, &QPushButton::clicked, this, [this, name, path, feedback] {
        const QString file = path->text().trimmed();
        if (!QFileInfo(file).isFile()) {
            feedback->setText(QStringLiteral("请选择一个有效图像文件"));
            return;
        }
        startReferenceImport(name, file);
    });
    panel->setGeometry(rect());
    panel->show();
    panel->raise();
}

void ReferenceManagePage::startReferenceImport(
    const QString& model, const QString& path, const cv::Mat& frame)
{
    auto prepared = std::make_shared<ReferenceImage>();
    prepared->modelName = model;
    prepared->image = frame.clone();
    FileOperation::run(
        this, QStringLiteral("正在导入基准图"),
        [prepared, path] {
            QString error;
            if (!path.isEmpty()) {
                QImageReader reader(path);
                const QSize size = reader.size();
                if (!size.isValid() || qint64(size.width()) * size.height() > 32000000)
                    return QStringLiteral("无法识别图像或尺寸超过 3200 万像素");
                prepared->image = cv::imread(path.toStdString(), cv::IMREAD_COLOR);
            }
            if (prepared->image.empty())
                return QStringLiteral("无法解码基准图");
            ReferenceImageManager::savePreparedReference(*prepared, &error);
            return error;
        },
        [this, prepared](const QString& error) {
            if (!error.isEmpty()) {
                _statusText->setText(error);
                if (_filePanel) {
                    const auto labels = _filePanel->findChildren<QLabel*>();
                    if (!labels.isEmpty())
                        labels.last()->setText(error);
                }
                return;
            }
            auto& manager = ReferenceImageManager::instance();
            manager.adoptPreparedReference(*prepared);
            manager.setCurrentModel(prepared->modelName);
            refreshModelList();
            if (_filePanel) {
                auto* panel = _filePanel;
                _filePanel = nullptr;
                panel->hide();
                panel->deleteLater();
            }
            _statusText->setText(QStringLiteral("已导入基准图：%1，请标记定位区域").arg(prepared->modelName));
            Q_EMIT referenceModelsChanged();
        });
}

void ReferenceManagePage::onCaptureLiveFrameClicked()
{
    if (FileOperation::busy(this))
        return;
    if (_latestLiveFrame.empty()) {
        _statusText->setText(QStringLiteral("尚未获取到实时图像"));
        return;
    }
    const QString name = _modelNameEdit->text().trimmed();
    if (!ReferenceImageManager::isValidModelName(name)) {
        _statusText->setText(QStringLiteral("请先填写有效的型号名称"));
        return;
    }
    startReferenceImport(name, QString(), _latestLiveFrame);
}

void ReferenceManagePage::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    if (_filePanel)
        _filePanel->setGeometry(rect());
}

void ReferenceManagePage::onLiveFrameReady(const cv::Mat& frame)
{
    if (!_cameraLeaseHeld || frame.empty())
        return;
    _latestLiveFrame = frame.clone();
    if (ReferenceImageManager::instance().currentModel().isEmpty()) {
        _canvas->setImage(_latestLiveFrame);
        _statusText->setText("实时预览中：可采集当前帧注册基准图");
    }
}

void ReferenceManagePage::onModelChanged(const QString& modelName)
{
    if (modelName.isEmpty()) {
        return;
    }
    ReferenceImageManager::instance().setCurrentModel(modelName);
    refreshCurrentModelView();
}

void ReferenceManagePage::onMarkRectToggled(bool checked)
{
    _markingRectFirstCorner = false;
    _canvas->setMarkingEnabled(checked);
}

void ReferenceManagePage::onCanvasPointMarked(const QPointF& imagePoint)
{
    const QString modelName = ReferenceImageManager::instance().currentModel();
    if (modelName.isEmpty()) {
        return;
    }

    if (_markRectButton->isChecked()) {
        if (!_markingRectFirstCorner) {
            _rectFirstCorner = imagePoint;
            _markingRectFirstCorner = true;
            _statusText->setText("已记录第一个角点,请点击对角的另一个角点");
            return;
        }

        const QRectF rect(_rectFirstCorner, imagePoint);
        ReferenceImageManager::instance().setReferenceRect(modelName, rect.normalized());
        _markingRectFirstCorner = false;
        _markRectButton->setChecked(false);
        refreshCurrentModelView();
    }
}

void ReferenceManagePage::selectModel(const QString& name)
{
    if (!ReferenceImageManager::instance().setCurrentModel(name))
        ReferenceImageManager::instance().setCurrentModel(QString());
    refreshModelList();
}
