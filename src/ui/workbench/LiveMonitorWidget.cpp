#include "LiveMonitorWidget.h"
#include "InspectionTrendWidget.h"
#include "InspectionPresentation.h"
#include <QResizeEvent>
#include <QComboBox>
#include <QCheckBox>
#include <QSignalBlocker>

#include "ImageInputPanel.h"
#include "../ImageCanvasWidget.h"
#include "infrastructure/camera/CameraStreamService.h"
#include "infrastructure/communication/TcpServerService.h"

#include <QCoreApplication>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QDialog>
#include <QPlainTextEdit>
#include <QFile>
#include <QStandardPaths>
#include <QJsonArray>
#include <QJsonDocument>
#include <QDateTime>
#include <QTimer>

LiveMonitorWidget::LiveMonitorWidget(QWidget* parent)
    : QWidget(parent)
{
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(18, 16, 18, 16);
    root->setSpacing(12);

    auto* header = new QHBoxLayout();
    header->setSpacing(12);
    _schemeTitle = new QLabel(QStringLiteral("尚未创建方案"), this);
    _schemeTitle->setObjectName("sectionTitle");
    header->addWidget(_schemeTitle);
    header->addStretch(1);
    _streamStatus = new QLabel(QStringLiteral("● 等待采集"), this);
    _streamStatus->setStyleSheet("color:#2e86c1;font-size:13px;font-weight:600;");
    header->addWidget(_streamStatus);
    auto* pauseButton = _pauseButton = new QPushButton(QStringLiteral("暂停运行"), this);
    connect(pauseButton, &QPushButton::clicked, this, [this, pauseButton] {
        toggleRun();
        pauseButton->setText(_paused ? QStringLiteral("继续运行") : QStringLiteral("暂停运行"));
    });
    header->addWidget(pauseButton);
    auto* once = new QPushButton(QStringLiteral("单次检测"), this);
    connect(once, &QPushButton::clicked, this, &LiveMonitorWidget::runOnceRequested);
    header->addWidget(once);
    _editSchemeButton = new QPushButton(QStringLiteral("删除方案"), this);
    _editSchemeButton->setEnabled(false);
    connect(_editSchemeButton, &QPushButton::clicked, this, &LiveMonitorWidget::deleteSchemeRequested);
    header->addWidget(_editSchemeButton);
    pauseButton->setFixedSize(104, 36);
    once->setFixedSize(104, 36);
    _editSchemeButton->setFixedSize(104, 36);
    root->addLayout(header);

    auto* center = new QHBoxLayout();
    center->setSpacing(14);
    auto* schemes = new QFrame(this);
    schemes->setObjectName("configPanel");
    schemes->setMinimumWidth(150);
    schemes->setMaximumWidth(190);
    auto* schemeLayout = new QVBoxLayout(schemes);
    schemeLayout->setContentsMargins(12, 12, 12, 12);
    schemeLayout->setSpacing(8);
    schemeLayout->addWidget(new QLabel(QStringLiteral("方案列表"), schemes));
    _schemeList = new QListWidget(schemes);
    connect(_schemeList, &QListWidget::currentTextChanged, this, [this](const QString& schemeName) {
        if (!schemeName.isEmpty())
            Q_EMIT schemeSelectionRequested(schemeName);
    });
    schemeLayout->addWidget(_schemeList, 1);
    auto* newSchemeButton = new QPushButton(QStringLiteral("+ 新建方案"), schemes);
    connect(newSchemeButton, &QPushButton::clicked, this, &LiveMonitorWidget::newSchemeRequested);
    schemeLayout->addWidget(newSchemeButton);
    _listEditSchemeButton = new QPushButton(QStringLiteral("⚙ 编辑当前方案"), schemes);
    _listEditSchemeButton->setEnabled(false);
    connect(_listEditSchemeButton, &QPushButton::clicked, this, &LiveMonitorWidget::editSchemeRequested);
    schemeLayout->addWidget(_listEditSchemeButton);
    auto* importSchemeButton = new QPushButton(QStringLiteral("导入方案"), schemes);
    connect(importSchemeButton, &QPushButton::clicked, this, &LiveMonitorWidget::importSchemeRequested);

    schemeLayout->addWidget(importSchemeButton);
    _exportSchemeButton = new QPushButton(QStringLiteral("保存为 YAML"), schemes);
    connect(_exportSchemeButton, &QPushButton::clicked, this, &LiveMonitorWidget::exportSchemeRequested);
    schemeLayout->addWidget(_exportSchemeButton);
    for (auto* button : { newSchemeButton, _listEditSchemeButton, importSchemeButton, _exportSchemeButton })
        button->setFixedHeight(36);
    center->addWidget(schemes);
    _canvas = new ImageCanvasWidget(this);
    // RK3576 触摸屏分辨率差异很大；预览必须可收缩，不能把底部运行按钮挤出屏幕。
    _canvas->setMinimumSize(320, 180);
    _canvas->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    _canvas->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    auto* previewColumn = new QWidget(this);
    auto* previewLayout = new QVBoxLayout(previewColumn);
    previewLayout->setContentsMargins(0, 0, 0, 0);
    previewLayout->setSpacing(10);
    auto* viewControls = new QHBoxLayout();
    viewControls->setSpacing(12);
    viewControls->addWidget(new QLabel(QStringLiteral("结果可视化"), this));
    _toolSelector = new QComboBox(this);
    _toolSelector->setMinimumWidth(150);
    _toolSelector->addItem(QStringLiteral("全部算法"), QString());
    viewControls->addWidget(_toolSelector);
    _testToolButton = new QPushButton(QStringLiteral("测试所选算法"), this);
    _testToolButton->setEnabled(false);
    viewControls->addWidget(_testToolButton);
    _liveView = new QCheckBox(QStringLiteral("实时原图"), this);
    viewControls->addWidget(_liveView);
    viewControls->addStretch(1);
    _toolSelector->setFixedHeight(36);
    _testToolButton->setFixedHeight(36);
    _liveView->setMinimumHeight(36);
    previewLayout->addLayout(viewControls);
    _frameInfo = new QLabel(QStringLiteral("等待检测结果帧 · 框与图像同步更新"), this);
    _frameInfo->setObjectName("muted");
    previewLayout->addWidget(_frameInfo);
    previewLayout->addWidget(_canvas, 1);
    connect(_toolSelector, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this] {
        _testToolButton->setEnabled(!_toolSelector->currentData().toString().isEmpty());
        _trend->selectTool(_toolSelector->currentData().toString());
        refreshVisualization();
    });
    connect(_liveView, &QCheckBox::toggled, this, [this] { refreshVisualization(); });
    connect(_testToolButton, &QPushButton::clicked, this, [this] {
        if (!_paused) {
            toggleRun();
            _pauseButton->setText(QStringLiteral("继续运行"));
        }
        _liveView->setChecked(false);
        Q_EMIT runToolRequested(_toolSelector->currentData().toString());
    });

    auto* resultFrame = new QFrame(previewColumn);
    resultFrame->setObjectName("configPanel");
    auto* resultLayout = new QVBoxLayout(resultFrame);
    resultLayout->setContentsMargins(12, 12, 12, 12);
    resultLayout->setSpacing(8);
    resultLayout->addWidget(new QLabel(QStringLiteral("本帧识别信息"), resultFrame));
    _toolResultTable = new QTableWidget(resultFrame);
    _toolResultTable->setColumnCount(5);
    _toolResultTable->setHorizontalHeaderLabels({ QStringLiteral("工具"), QStringLiteral("判定"),
        QStringLiteral("输入"), QStringLiteral("输出"), QStringLiteral("耗时") });
    auto* resultHeader = _toolResultTable->horizontalHeader();
    resultHeader->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    _toolResultTable->horizontalHeaderItem(4)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    resultHeader->setMinimumSectionSize(52);
    resultHeader->setSectionResizeMode(QHeaderView::Interactive);
    _toolResultTable->setColumnWidth(0, 105);
    _toolResultTable->setColumnWidth(1, 66);
    _toolResultTable->setColumnWidth(2, 145);
    _toolResultTable->setColumnWidth(4, 78);
    resultHeader->setSectionResizeMode(3, QHeaderView::Stretch);
    _toolResultTable->verticalHeader()->setDefaultSectionSize(32);
    _toolResultTable->setWordWrap(false);
    _toolResultTable->verticalHeader()->setVisible(false);
    _toolResultTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _toolResultTable->setMinimumHeight(100);
    _toolResultTable->setMaximumHeight(160);
    _toolResultTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    _toolResultTable->setSelectionMode(QAbstractItemView::SingleSelection);
    connect(_toolResultTable, &QTableWidget::cellClicked, this, [this](int row, int) {
        const auto tools = _lastResult.value("tools").toArray();
        if (row < tools.size()) {
            int index = _toolSelector->findData(tools[row].toObject().value("id").toString());
            if (index >= 0)
                _toolSelector->setCurrentIndex(index);
        }
    });
    resultLayout->addWidget(_toolResultTable);
    _toolDetail = new QLabel(QStringLiteral("点击算法行查看详情，选择算法后可单独测试。"), this);
    _toolDetail->setWordWrap(true);
    _toolDetail->setMaximumHeight(46);
    _toolDetail->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
    _toolDetail->setObjectName("muted");
    resultLayout->addWidget(_toolDetail);

    auto* side = new QFrame(this);
    side->setObjectName("configPanel");
    side->setFixedWidth(230);
    auto* sideLayout = new QVBoxLayout(side);
    sideLayout->setContentsMargins(12, 12, 12, 12);
    sideLayout->setSpacing(6);
    auto* stateTitle = new QLabel(QStringLiteral("当前结果"), side);
    stateTitle->setObjectName("sectionTitle");
    sideLayout->addWidget(stateTitle);
    _resultBadge = new QLabel(QStringLiteral("等待"), side);
    _resultBadge->setAlignment(Qt::AlignCenter);
    _resultBadge->setMinimumHeight(46);
    _resultBadge->setStyleSheet(
        "background:#e2e8f0;color:#475569;font-size:26px;font-weight:700;border-radius:5px;");
    sideLayout->addWidget(_resultBadge);
    _triggerStatus = new QLabel(side);
    _triggerStatus->setWordWrap(true);
    _triggerStatus->setObjectName("muted");
    sideLayout->addWidget(_triggerStatus);
    _resultStatus = new QLabel(QStringLiteral("总数  0\nOK  0    NG  0\n良率  --"), side);
    _resultStatus->setObjectName("muted");
    _resultStatus->setStyleSheet("font-size:12px;");
    sideLayout->addWidget(_resultStatus);
    _trend = new InspectionTrendWidget(side);
    sideLayout->addWidget(_trend);

    _systemUsageLabel = new QLabel(QStringLiteral("整机 CPU --%  ·  内存 --%"), side);
    _systemUsageLabel->setObjectName("muted");
    _systemUsageLabel->setStyleSheet("font-size:12px;");
    sideLayout->addWidget(_systemUsageLabel);
    _systemUsageTimer = new QTimer(this);
    _systemUsageTimer->setInterval(1000);
    connect(_systemUsageTimer, &QTimer::timeout, this, &LiveMonitorWidget::updateSystemUsage);
    updateSystemUsage();
    _systemUsageTimer->start();

    // legend->setObjectName("muted");sideLayout->addWidget(legend);
    sideLayout->addStretch(1);
    auto* history = new QPushButton(QStringLiteral("查看最近结果"), side);
    connect(history, &QPushButton::clicked, this, [this] {
        if (!_historyPanel) {
            _historyPanel = new QFrame(this);
            _historyPanel->setObjectName("configPanel");
            _historyPanel->setAutoFillBackground(true);
            auto* layout = new QVBoxLayout(_historyPanel);
            layout->addWidget(
                new QLabel(QStringLiteral("最近检测结果（最多100条，最新在前）"), _historyPanel));
            auto* table = new QTableWidget(_historyPanel);
            table->setColumnCount(9);
            table->setHorizontalHeaderLabels(
                { QStringLiteral("时间"), QStringLiteral("方案"), QStringLiteral("总判定"),
                    QStringLiteral("工具"), QStringLiteral("工具判定"), QStringLiteral("输入"),
                    QStringLiteral("输出 / 说明"), QStringLiteral("工具 ms"), QStringLiteral("总 ms") });
            table->setEditTriggers(QAbstractItemView::NoEditTriggers);
            table->setWordWrap(false);
            table->verticalHeader()->hide();
            table->setSelectionBehavior(QAbstractItemView::SelectRows);
            table->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            table->horizontalHeaderItem(7)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
            table->horizontalHeaderItem(8)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
            table->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
            const QList<int> widths { 145, 100, 65, 100, 75, 160, 260, 80, 80 };
            for (int c = 0; c < widths.size(); ++c)
                table->setColumnWidth(c, widths[c]);
            table->horizontalHeader()->setSectionResizeMode(6, QHeaderView::Stretch);
            layout->addWidget(table);
            auto* close = new QPushButton(QStringLiteral("返回实时画面"), _historyPanel);
            connect(close, &QPushButton::clicked, _historyPanel, &QWidget::hide);
            layout->addWidget(close);
        }
        auto* table = _historyPanel->findChild<QTableWidget*>();
        QFile f(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/results/recent.jsonl");
        QList<QJsonObject> records;
        if (f.open(QIODevice::ReadOnly)) {
            while (!f.atEnd()) {
                auto object = QJsonDocument::fromJson(f.readLine()).object();
                if (!object.isEmpty())
                    records.append(object);
                if (records.size() > 100)
                    records.removeFirst();
            }
        }
        table->setRowCount(0);
        for (int index = records.size() - 1; index >= 0; --index) {
            const auto record = records[index];
            auto tools = record.value("tools").toArray();
            if (tools.isEmpty())
                tools.append(QJsonObject { { "name", QStringLiteral("方案") },
                    { "status", record.value("status") }, { "error", record.value("error") } });
            for (const auto& value : tools) {
                const auto tool = value.toObject();
                const QStringList cells { QDateTime::fromString(
                                              record.value("timestamp").toString(), Qt::ISODateWithMs)
                                              .toLocalTime()
                                              .toString("MM-dd HH:mm:ss.zzz"),
                    record.value("scheme").toString(), record.value("status").toString(),
                    tool.value("name").toString(), tool.value("status").toString(),
                    InspectionPresentation::input(tool), InspectionPresentation::output(tool),
                    tool.contains("elapsedMs") ? QString::number(tool.value("elapsedMs").toDouble())
                                               : QStringLiteral("—"),
                    QString::number(record.value("elapsedMs").toDouble()) };
                const int row = table->rowCount();
                table->insertRow(row);
                for (int col = 0; col < cells.size(); ++col) {
                    auto* item = new QTableWidgetItem(cells[col]);
                    item->setToolTip(cells[col]);
                    if (col == 2 || col == 4)
                        item->setForeground(QColor(cells[col] == "OK" ? "#15803d"
                                : cells[col] == "NG"                  ? "#dc2626"
                                                                      : "#d97706"));
                    item->setTextAlignment((col >= 7 ? Qt::AlignRight : Qt::AlignLeft) | Qt::AlignVCenter);
                    table->setItem(row, col, item);
                }
            }
        }
        _historyPanel->setGeometry(rect());
        _historyPanel->show();
        _historyPanel->raise();
    });
    history->setFixedHeight(36);
    sideLayout->addWidget(history);
    root->addLayout(center, 1);
    previewLayout->addWidget(resultFrame);
    center->addWidget(previewColumn, 1);
    center->addWidget(side);

    connect(&CameraStreamService::instance(), &CameraStreamService::frameReady, this,
        &LiveMonitorWidget::onFrameReady);
    connect(&CameraStreamService::instance(), &CameraStreamService::streamError, this,
        &LiveMonitorWidget::onStreamError);
}

void LiveMonitorWidget::setSchemes(const QStringList& schemes, const QString& currentScheme)
{
    _schemeList->blockSignals(true);
    _schemeList->clear();
    _schemeList->addItems(schemes);
    const int currentIndex = schemes.indexOf(currentScheme);
    _schemeList->setCurrentRow(currentIndex);
    _schemeList->blockSignals(false);

    const bool hasCurrentScheme = currentIndex >= 0;
    _exportSchemeButton->setEnabled(hasCurrentScheme);
    _editSchemeButton->setEnabled(hasCurrentScheme);
    _listEditSchemeButton->setEnabled(hasCurrentScheme);
    _schemeTitle->setText(hasCurrentScheme ? QStringLiteral("当前方案  /  %1").arg(currentScheme)
                                           : QStringLiteral("尚未创建方案"));
    if (_statsScheme != currentScheme) {
        _statsScheme = currentScheme;
        _total = _ok = _ng = _errors = 0;
        _trend->clear();
        _resultBadge->setText(QStringLiteral("等待"));
        _resultBadge->setStyleSheet("background:#e2e8f0;color:#475569;font-size:26px;font-weight:700;");
    }
    if (!_total)
        _resultStatus->setText(hasCurrentScheme
                ? QStringLiteral("总数  0\nOK  0    NG  0\n良率  --")
                : QStringLiteral("请先创建并保存方案\n\n主控不会显示未保存的默认方案"));
}

void LiveMonitorWidget::setCurrentSchemeTools(const QList<SchemeTool>& tools)
{
    _toolResultTable->clearSpans();
    _lastResult = { };
    _resultFrame.release();
    _canvas->setAnnotations({ });
    {
        QSignalBlocker blocker(_toolSelector);
        _toolSelector->clear();
        _toolSelector->addItem(QStringLiteral("全部算法"), QString());
        for (const auto& tool : tools)
            _toolSelector->addItem(tool.name, tool.id);
    }
    _testToolButton->setEnabled(false);
    _trend->selectTool(QString());
    refreshVisualization();
    _toolResultTable->setRowCount(tools.size());
    for (int row = 0; row < tools.size(); ++row) {
        const SchemeTool& tool = tools.at(row);
        _toolResultTable->setItem(row, 0, new QTableWidgetItem(tool.name));
        _toolResultTable->setItem(row, 1, new QTableWidgetItem(QStringLiteral("等待运行")));
        _toolResultTable->setItem(row, 2, new QTableWidgetItem(QStringLiteral("--")));
        _toolResultTable->setItem(row, 3, new QTableWidgetItem(QStringLiteral("--")));
        _toolResultTable->setItem(row, 4, new QTableWidgetItem(QStringLiteral("-- ms")));
        _toolResultTable->item(row, 4)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    }
    if (tools.isEmpty()) {
        _toolResultTable->setRowCount(1);
        _toolResultTable->setSpan(0, 0, 1, 5);
        _toolResultTable->setItem(
            0, 0, new QTableWidgetItem(QStringLiteral("尚未配置工具，请进入方案编辑添加工具")));
    }
}

LiveMonitorWidget::~LiveMonitorWidget()
{
    stopStream();
}

void LiveMonitorWidget::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    if (!_paused)
        startStream();
}

void LiveMonitorWidget::hideEvent(QHideEvent* event)
{
    stopStream();
    QWidget::hideEvent(event);
}

void LiveMonitorWidget::updateSystemUsage()
{
    const auto usage = _systemResources.sample();
    const QString cpu
        = usage.cpuPercent >= 0 ? QString::number(usage.cpuPercent, 'f', 1) : QStringLiteral("--");
    const QString memory
        = usage.memoryPercent >= 0 ? QString::number(usage.memoryPercent, 'f', 1) : QStringLiteral("异常");

    _systemUsageLabel->setText(QStringLiteral("整机 CPU %1%  ·  内存 %2%").arg(cpu, memory));
    _systemUsageLabel->setToolTip(QStringLiteral("更新时间：%1\n已用内存：%2 MiB / %3 MiB")
            .arg(QDateTime::currentDateTime().toString("HH:mm:ss"))
            .arg(usage.memoryUsedKiB / 1024.0, 0, 'f', 1)
            .arg(usage.memoryTotalKiB / 1024.0, 0, 'f', 1));
}

void LiveMonitorWidget::startStream()
{
    if (_cameraLeaseHeld)
        return;

    CameraStreamService::instance().acquire();
    _cameraLeaseHeld = true;
    _streamStatus->setText(QStringLiteral("● 正在采集"));
    _streamStatus->setStyleSheet("color:#2e86c1;font-size:13px;font-weight:600;");
}

void LiveMonitorWidget::stopStream()
{
    if (!_cameraLeaseHeld)
        return;

    CameraStreamService::instance().release();
    _cameraLeaseHeld = false;
}

void LiveMonitorWidget::toggleRun()
{
    _paused = !_paused;
    Q_EMIT pausedChanged(_paused);
    if (_paused) {
        stopStream();
        _streamStatus->setText(QStringLiteral("● 已暂停"));
        _streamStatus->setStyleSheet("color:#aeb5c0;font-size:13px;");
    } else {
        startStream();
    }
}

void LiveMonitorWidget::onFrameReady(const cv::Mat& frame)
{
    // 切到文件夹输入后服务会保留短暂的停流窗口；该窗口内的相机帧不能覆盖
    // 用户已选文件，也不能成为后续检测的输入。
    if (!_cameraLeaseHeld)
        return;

    _currentFrame = frame.clone();
    if (_liveView->isChecked() || _resultFrame.empty())
        _canvas->setImage(frame);
}

void LiveMonitorWidget::onStreamError(const QString& message)
{
    if (!_cameraLeaseHeld)
        return;

    _streamStatus->setText(QStringLiteral("● %1").arg(message));
    _streamStatus->setStyleSheet("color:#ef6b73;font-size:13px;");
}

void LiveMonitorWidget::onFolderImageReady(const cv::Mat& image, const QString& fileName)
{
    _currentFrame = image.clone();
    _currentSource = fileName;
    _canvas->setImage(image);
    _streamStatus->setText(QStringLiteral("● 文件输入  %1").arg(fileName));
    _streamStatus->setStyleSheet("color:#2e86c1;font-size:13px;font-weight:600;");
}

void LiveMonitorWidget::switchToCameraSource()
{
    if (isVisible())
        startStream();
}

void LiveMonitorWidget::switchToFolderSource()
{
    stopStream();
    _streamStatus->setText(QStringLiteral("● 图片文件夹模式"));
    _streamStatus->setStyleSheet("color:#5da9e9;font-size:13px;");
}

void LiveMonitorWidget::setTriggerSettings(bool enabled, int intervalMs, bool modbus)
{
    _triggerStatus->setText(modbus ? QStringLiteral("Modbus 触发 · 等待上位机指令")
            : enabled              ? QStringLiteral("内部触发 · %1 ms").arg(intervalMs)
                                   : QStringLiteral("手动触发"));
}

void LiveMonitorWidget::showInspection(const QJsonObject& result, const cv::Mat& image, bool toolTest)
{
    _lastResult = result;
    _lastResult["toolTest"] = toolTest;
    _resultFrame = image.clone();
    const QString final = result.value("status").toString();
    if (!toolTest) {
        ++_total;
        if (final == "OK")
            ++_ok;
        else if (final == "NG")
            ++_ng;
        else
            ++_errors;
        _trend->append(result);
    }
    _resultBadge->setText((toolTest ? QStringLiteral("测试 ") : QString()) + final);
    const QString color = final == "OK" ? "#15803d" : final == "NG" ? "#dc2626" : "#d97706";
    _resultBadge->setStyleSheet(
        QString("background:%1;color:white;font-size:24px;font-weight:700;border-radius:5px;").arg(color));
    _resultStatus->setText(QStringLiteral("总数 %1\nOK %2    NG %3\n错误 %4\n良率 %5%\n本次 %6 ms")
            .arg(_total)
            .arg(_ok)
            .arg(_ng)
            .arg(_errors)
            .arg(_total ? 100.0 * _ok / _total : 0, 0, 'f', 1)
            .arg(result.value("elapsedMs").toDouble()));
    _toolResultTable->clearSpans();
    const QJsonArray tools = result.value("tools").toArray();
    _toolResultTable->setRowCount(tools.size());
    for (int row = 0; row < tools.size(); ++row) {
        const auto t = tools[row].toObject();
        const auto fields = t.value("fields").toObject();
        const QStringList cells { t.value("name").toString(), t.value("status").toString(),
            InspectionPresentation::input(t), InspectionPresentation::output(t),
            QString::number(t.value("elapsedMs").toDouble()) + " ms" };
        for (int col = 0; col < cells.size(); ++col) {
            auto* item = new QTableWidgetItem(cells[col]);
            item->setToolTip(cells[col]);
            if (col == 1) {
                const QString st = t.value("status").toString();
                item->setForeground(QColor(st == "OK" ? "#15803d" : st == "NG" ? "#dc2626" : "#d97706"));
                auto font = item->font();
                font.setBold(true);
                item->setFont(font);
            }
            item->setTextAlignment((col == 4 ? Qt::AlignRight : Qt::AlignLeft) | Qt::AlignVCenter);
            _toolResultTable->setItem(row, col, item);
        }
    }
    refreshVisualization();
}

void LiveMonitorWidget::refreshVisualization()
{
    const QString selected = _toolSelector->currentData().toString();
    QStringList details;
    QList<ImageAnnotation> annotations;
    for (const auto& value : _lastResult.value("tools").toArray()) {
        const auto tool = value.toObject();
        if (!selected.isEmpty() && tool.value("id").toString() != selected)
            continue;
        const auto fields = tool.value("fields").toObject();
        const QString status = tool.value("status").toString();
        QString text = tool.value("name").toString() + " · " + status;
        if (fields.contains("score"))
            text += QStringLiteral(" · 分数 %1 · 角度 %2°")
                        .arg(fields.value("score").toDouble(), 0, 'f', 3)
                        .arg(fields.value("angle").toDouble(), 0, 'f', 1);
        if (fields.contains("text"))
            text += QStringLiteral(" · %1 · 置信度 %2%")
                        .arg(fields.value("text").toString())
                        .arg(fields.value("confidence").toDouble() * 100, 0, 'f', 1);
        if (!tool.value("error").toString().isEmpty())
            text += " · " + tool.value("error").toString();
        details << text;
        QColor color = status == "OK"
            ? (tool.value("kind") == "template" ? QColor("#38bdf8") : QColor("#4ade80"))
            : QColor("#fb923c");
        auto polygon = [](const QJsonArray& data) {
            QPolygonF p;
            for (const auto& v : data) {
                const auto a = v.toArray();
                if (a.size() == 2)
                    p << QPointF(a[0].toDouble(), a[1].toDouble());
            }
            return p;
        };
        const QString marker = QStringLiteral("%1 · %2").arg(tool.value("name").toString(), status);
        annotations.append({ polygon(fields.value("polygon").toArray()), marker, color });
        const auto regions = fields.value("regions").toArray();
        for (const auto& region : regions) {
            const auto r = region.toObject();
            QString label = r.value("label").toString(r.value("text").toString());
            if (label.isEmpty() && r.contains("classId"))
                label = QStringLiteral("类别 %1").arg(r["classId"].toInt());
            if (r.contains("classId") && r.value("confidence").isDouble())
                label += QStringLiteral(" %1%").arg(r["confidence"].toDouble() * 100, 0, 'f', 1);
            annotations.append({ polygon(r.value("polygon").toArray()), label, color });
        }
    }
    _toolDetail->setText(details.isEmpty() ? QStringLiteral("点击算法行查看详情，选择算法后可单独测试。")
                                           : details.join("  |  "));
    _toolDetail->setToolTip(details.join("\n"));
    if (_liveView->isChecked()) {
        if (!_currentFrame.empty())
            _canvas->setImage(_currentFrame);
        _canvas->setAnnotations({ });
        _frameInfo->setText(QStringLiteral("实时原图 · 检测标注请取消“实时原图”"));
    } else {
        if (!_resultFrame.empty())
            _canvas->setImage(_resultFrame);
        _canvas->setAnnotations(annotations);
        _frameInfo->setText(_lastResult.isEmpty()
                ? QStringLiteral("等待检测结果帧 · 框与图像同步更新")
                : QStringLiteral("%1 · %2 · %3 ms%4")
                      .arg(_lastResult.value("toolTest").toBool() ? QStringLiteral("单算子测试帧")
                                                                  : QStringLiteral("检测结果帧"),
                          QDateTime::fromString(_lastResult.value("timestamp").toString(), Qt::ISODateWithMs)
                              .toLocalTime()
                              .toString("HH:mm:ss.zzz"))
                      .arg(_lastResult.value("elapsedMs").toDouble())
                      .arg(_paused ? QStringLiteral(" · 已暂停") : QString()));
    }
}

void LiveMonitorWidget::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    if (_historyPanel)
        _historyPanel->setGeometry(rect());
}
