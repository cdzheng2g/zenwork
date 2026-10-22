#include "ui/workbench/WorkbenchWidget.h"
#include "ui/FileOperation.h"
#include "application/scheme/SchemePackage.h"
#include "application/InternalTrigger.h"
#include "infrastructure/camera/CameraController.h"
#include "infrastructure/camera/CameraParamsJson.h"
#include "infrastructure/reference/ReferenceImageManager.h"

#include <QCheckBox>
#include <QDir>
#include <QFileInfo>
#include <QFileSystemModel>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QResizeEvent>
#include <QStandardPaths>
#include <QTreeView>
#include <QUuid>
#include <QVBoxLayout>
#include <memory>

namespace {
QLabel* makeLabel(const QString& text, const QString& objectName = { })
{
    auto* label = new QLabel(text);
    if (!objectName.isEmpty())
        label->setObjectName(objectName);
    return label;
}
} // namespace

void WorkbenchWidget::exportSchemeFile(
    const QString& path, bool includeModels, std::function<void(const QString&)> finished)
{
    const auto name = _schemeCatalog.currentSchemeName();
    if (name.isEmpty()) {
        finished(QStringLiteral("请先保存一个方案"));
        return;
    }
    SchemePackage package;
    package.name = name;
    package.settings = _schemeCatalog.settings(name);
    package.tools = _schemeCatalog.tools(name);
    if (package.settings.camera.isEmpty())
        package.settings.camera = cameraParamsJson(CameraController::instance().currentParams());
    if (const auto* ref = ReferenceImageManager::instance().referenceImage(package.settings.referenceModel))
        package.reference = *ref;

    const QString message
        = includeModels ? QStringLiteral("正在导出完整迁移方案") : QStringLiteral("正在导出轻量 YAML 方案");
    FileOperation::run(
        this, message,
        [package, path, includeModels] {
            QString error;
            package.save(path, &error, includeModels);
            return error;
        },
        finished);
}

void WorkbenchWidget::importSchemeFile(const QString& path, std::function<void(const QString&)> finished)
{
    auto package = std::make_shared<SchemePackage>();
    const auto catalog = _schemeCatalog;
    const QString resources
        = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/scheme_resources";
    const QString referenceId = "import-" + QUuid::createUuid().toString(QUuid::WithoutBraces);
    FileOperation::run(
        this, QStringLiteral("正在导入方案、校验并恢复资源"),
        [package, path, catalog, resources, referenceId] {
            QString error;
            if (!SchemePackage::load(path, *package, &error))
                return error;
            const auto name = package->name;
            int suffix = 1;
            while (catalog.schemeNames().contains(package->name))
                package->name = name.left(48) + QStringLiteral("（导入 %1）").arg(suffix++);
            SchemeRuntimeSettings installed;
            if (!package->installResources(resources, installed, &error))
                return error;
            if (!package->reference.image.empty()) {
                package->reference.modelName = referenceId;
                if (!ReferenceImageManager::savePreparedReference(package->reference, &error))
                    return error;
                installed.referenceModel = referenceId;
            } else {
                installed.referenceModel.clear();
            }
            package->settings = installed;
            package->save(catalog.portablePath(package->name), &error);
            return error;
        },
        [this, package, finished](const QString& failure) {
            QString error = failure;
            if (!error.isEmpty()) {
                finished(error);
                return;
            }
            if (!_schemeCatalog.saveComplete(package->name, package->settings, package->tools, &error)) {
                finished(error);
                return;
            }
            if (!package->reference.image.empty())
                ReferenceImageManager::instance().adoptPreparedReference(package->reference);
            refreshSchemeControl();
            _statusLabel->setText(QStringLiteral("已导入方案：%1").arg(package->name));
            finished(QString());
        });
}

void WorkbenchWidget::showSchemeFilePanel(bool exporting)
{
    if (_filePanel || !_editingSchemeName.isEmpty())
        return;
    if (exporting && _schemeCatalog.currentSchemeName().isEmpty())
        return;
    _coordinator->setFileBusy(true);

    auto* panel = _filePanel = new QFrame(this);
    panel->setObjectName("configPanel");
    panel->setAutoFillBackground(true);
    auto* layout = new QVBoxLayout(panel);
    layout->setContentsMargins(24, 20, 24, 20);
    layout->addWidget(makeLabel(
        exporting ? QStringLiteral("保存方案为 YAML") : QStringLiteral("导入 YAML 方案"), "sectionTitle"));
    auto* description = makeLabel(exporting
            ? QStringLiteral("默认保存工具参数和基准图；OCR 模型按指纹在设备上共用。")
            : QStringLiteral("支持轻量方案、完整迁移方案及旧版 YAML；轻量方案需要设备已有相同模型。"),
        "muted");
    description->setWordWrap(true);
    layout->addWidget(description);

    auto* model = new QFileSystemModel(panel);
    model->setRootPath("/");
    model->setNameFilters({ "*.yaml", "*.yml" });
    model->setNameFilterDisables(false);
    auto* tree = new QTreeView(panel);
    tree->setModel(model);
    tree->setRootIndex(model->index("/"));
    tree->setColumnWidth(0, 420);
    for (int column = 1; column <= 3; ++column)
        tree->hideColumn(column);
    tree->setCurrentIndex(model->index(QDir::homePath()));
    tree->expand(model->index(QDir::homePath()));
    layout->addWidget(tree, 1);

    auto* pathEdit = new QLineEdit(panel);
    const QString initial = QDir::homePath() + "/" + _schemeCatalog.currentSchemeName() + ".yaml";
    pathEdit->setText(exporting ? initial : QDir::homePath() + "/");
    layout->addWidget(pathEdit);

    QCheckBox* includeModels = nullptr;
    if (exporting) {
        includeModels = new QCheckBox(QStringLiteral("包含 OCR 模型（完整迁移到未安装模型的设备）"), panel);
        includeModels->setChecked(false);
        layout->addWidget(includeModels);
    }

    auto* feedback = makeLabel(QStringLiteral("选择文件或输入完整路径；保存时可指定 U 盘目录。"), "muted");
    feedback->setWordWrap(true);
    layout->addWidget(feedback);
    auto* actions = new QHBoxLayout();
    actions->addStretch();
    auto* cancel = new QPushButton(QStringLiteral("返回"), panel);
    auto* accept = new QPushButton(exporting ? QStringLiteral("保存 YAML") : QStringLiteral("导入"), panel);
    actions->addWidget(cancel);
    actions->addWidget(accept);
    layout->addLayout(actions);

    connect(tree, &QTreeView::clicked, panel, [model, pathEdit, exporting, this](const QModelIndex& index) {
        QString path = model->filePath(index);
        if (model->isDir(index)) {
            if (!exporting)
                return;
            path += "/" + _schemeCatalog.currentSchemeName() + ".yaml";
        }
        pathEdit->setText(path);
    });

    auto close = [this, panel] {
        _filePanel = nullptr;
        panel->hide();
        panel->deleteLater();
        applyTriggerSettings();
    };
    connect(cancel, &QPushButton::clicked, this, close);
    connect(pathEdit, &QLineEdit::textChanged, accept,
        [accept] { accept->setProperty("overwritePath", QString()); });
    connect(accept, &QPushButton::clicked, this,
        [this, panel, pathEdit, feedback, accept, exporting, includeModels, close] {
            QString path = pathEdit->text().trimmed();
            if (!QDir::isAbsolutePath(path)) {
                feedback->setText(QStringLiteral("请输入完整文件路径"));
                return;
            }
            if (exporting && !path.endsWith(".yaml", Qt::CaseInsensitive)
                && !path.endsWith(".yml", Qt::CaseInsensitive))
                path += ".yaml";
            if (exporting && QFileInfo::exists(path)
                && accept->property("overwritePath").toString() != path) {
                accept->setProperty("overwritePath", path);
                feedback->setText(QStringLiteral("文件已存在，再次点击“保存 YAML”确认覆盖：%1").arg(path));
                return;
            }
            panel->hide();
            auto finished = [this, panel, feedback, exporting, path, close](const QString& error) {
                if (!error.isEmpty()) {
                    feedback->setText(error);
                    panel->show();
                    panel->raise();
                    return;
                }
                if (exporting) {
                    _statusLabel->setText(QStringLiteral("YAML 已保存：%1").arg(path));
                    _statusLabel->setToolTip(path);
                }
                close();
            };
            if (exporting)
                exportSchemeFile(path, includeModels->isChecked(), finished);
            else
                importSchemeFile(path, finished);
        });
    panel->setGeometry(rect());
    panel->show();
    panel->raise();
}

void WorkbenchWidget::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    if (_filePanel)
        _filePanel->setGeometry(rect());
}
