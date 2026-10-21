#include "AlgorithmConfigPage.h"

#include "../ImageCanvasWidget.h"
#include "ui/operators/OperatorEditorRegistry.h"
#include "ui/operators/RuleSetEditor.h"
#include <QTabWidget>
#include "infrastructure/reference/ReferenceImageManager.h"
#include "application/operators/OperatorRegistry.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QScrollArea>
#include <QShowEvent>
#include <QSignalBlocker>
#include <QStackedWidget>
#include <QUuid>
#include <QVBoxLayout>

AlgorithmConfigPage::AlgorithmConfigPage(QWidget* parent)
    : QWidget(parent)
{
    initContent();
}

void AlgorithmConfigPage::initContent()
{
    setObjectName(QStringLiteral("algorithmConfigPage"));
    setStyleSheet(QStringLiteral(
        "QFrame#operatorPanel, QFrame#operatorCard { background:#fff; border:1px solid #dce2e8; "
        "border-radius:8px; }"
        "QListWidget#configuredOperatorList { border:0; background:transparent; outline:0; }"
        "QListWidget#configuredOperatorList::item { padding:16px 12px; margin-bottom:6px; border:1px solid "
        "#dce2e8; border-radius:7px; }"
        "QListWidget#configuredOperatorList::item:selected { color:#a91427; background:#fff7f8; border:2px "
        "solid #a91427; }"
        "QPushButton#addOperatorButton { color:#fff; background:#a91427; border:1px solid #a91427; }"
        "QPushButton#addOperatorButton:hover { background:#8e1021; }"
        "QLabel#operatorTitle { font-size:20px; font-weight:600; color:#243142; }"));

    auto* root = new QHBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(14);

    auto* installed = new QFrame(this);
    installed->setObjectName(QStringLiteral("operatorPanel"));
    installed->setFixedWidth(260);
    auto* installedLayout = new QVBoxLayout(installed);
    installedLayout->setContentsMargins(16, 16, 16, 16);
    installedLayout->setSpacing(12);
    auto* installedHeader = new QHBoxLayout();
    auto* installedTitle = new QLabel(QStringLiteral("已添加算子"), installed);
    installedTitle->setObjectName(QStringLiteral("operatorTitle"));
    installedTitle->setFixedHeight(36);
    installedHeader->addWidget(installedTitle);
    installedHeader->addStretch();
    _toolCount = new QLabel(installed);
    installedHeader->addWidget(_toolCount);
    installedLayout->addLayout(installedHeader);

    auto* addButton = new QPushButton(QStringLiteral("＋ 添加算子"), installed);
    addButton->setObjectName(QStringLiteral("addOperatorButton"));
    addButton->setFixedHeight(44);
    connect(addButton, &QPushButton::clicked, this, &AlgorithmConfigPage::showLibrary);
    installedLayout->addWidget(addButton);

    _configuredToolList = new QListWidget(installed);
    _configuredToolList->setObjectName(QStringLiteral("configuredOperatorList"));
    _configuredToolList->setSpacing(5);
    _configuredToolList->setSelectionMode(QAbstractItemView::SingleSelection);
    connect(_configuredToolList, &QListWidget::currentRowChanged, this, &AlgorithmConfigPage::showToolEditor);
    installedLayout->addWidget(_configuredToolList, 1);
    auto* removeButton = new QPushButton(QStringLiteral("移除选中算子"), installed);
    removeButton->setFixedHeight(44);
    connect(removeButton, &QPushButton::clicked, this, &AlgorithmConfigPage::removeSelectedTool);
    installedLayout->addWidget(removeButton);
    root->addWidget(installed);

    _contentStack = new QStackedWidget(this);
    auto* editor = new QFrame(_contentStack);
    editor->setObjectName(QStringLiteral("operatorPanel"));
    auto* editorLayout = new QVBoxLayout(editor);
    editorLayout->setContentsMargins(16, 16, 16, 16);
    editorLayout->setSpacing(12);
    _editorTitle = new QLabel(QStringLiteral("算子参数"), editor);
    _editorTitle->setObjectName(QStringLiteral("operatorTitle"));
    _editorTitle->setFixedHeight(36);
    editorLayout->addWidget(_editorTitle);
    _operatorStack = new QStackedWidget(editor);
    auto* empty = new QLabel(QStringLiteral("选择左侧算子，或点击“添加算子”"), _operatorStack);
    empty->setAlignment(Qt::AlignCenter);
    _operatorStack->addWidget(empty);
    editorLayout->addWidget(_operatorStack, 1);
    _contentStack->addWidget(editor);

    auto* library = new QFrame(_contentStack);
    library->setObjectName(QStringLiteral("operatorPanel"));
    auto* libraryLayout = new QVBoxLayout(library);
    libraryLayout->setContentsMargins(16, 16, 16, 16);
    libraryLayout->setSpacing(12);
    auto* libraryHeader = new QHBoxLayout();
    auto* libraryTitle = new QLabel(QStringLiteral("算子库"), library);
    libraryTitle->setObjectName(QStringLiteral("operatorTitle"));
    libraryTitle->setFixedHeight(36);
    libraryHeader->addWidget(libraryTitle);
    libraryHeader->addStretch();
    auto* backButton = new QPushButton(QStringLiteral("返回参数设置"), library);
    connect(backButton, &QPushButton::clicked, this,
        [this] { showToolEditor(_configuredToolList->currentRow()); });
    backButton->setFixedHeight(36);
    libraryHeader->addWidget(backButton);
    libraryLayout->addLayout(libraryHeader);
    auto* cards = new QHBoxLayout();
    cards->setSpacing(14);
    for (const auto& item : OperatorRegistry::definitions())
        cards->addWidget(createLibraryCard(item.name, item.symbol, item.kind));
    libraryLayout->addLayout(cards);
    libraryLayout->addStretch();
    _contentStack->addWidget(library);
    root->addWidget(_contentStack, 1);
    setConfiguredTools({ });
}

QWidget* AlgorithmConfigPage::createLibraryCard(
    const QString& title, const QString& symbol, const QString& kind)
{
    auto* card = new QFrame(this);
    card->setObjectName(QStringLiteral("operatorCard"));
    auto* layout = new QVBoxLayout(card);
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(16);
    auto* icon = new QLabel(symbol, card);
    icon->setAlignment(Qt::AlignCenter);
    icon->setFixedSize(52, 52);
    icon->setStyleSheet(QStringLiteral("background:#f9e7ea;color:#a91427;font-size:26px;border-radius:9px;"));
    layout->addWidget(icon);
    auto* name = new QLabel(title, card);
    name->setObjectName(QStringLiteral("operatorTitle"));
    layout->addWidget(name);
    layout->addStretch();
    auto* addButton = new QPushButton(QStringLiteral("添加到方案"), card);
    connect(addButton, &QPushButton::clicked, this, [this, kind] { addTool(kind); });
    layout->addWidget(addButton, 0, Qt::AlignRight);
    _addButtons.insert(kind, addButton);
    return card;
}

void AlgorithmConfigPage::showLibrary()
{
    _contentStack->setCurrentIndex(1);
}

void AlgorithmConfigPage::showToolEditor(int row)
{
    if (row < 0 || row >= _configuredTools.size()) {
        _editorTitle->setText(QStringLiteral("算子参数"));
        _operatorStack->setCurrentIndex(0);
    } else {
        const auto& tool = _configuredTools.at(row);
        _editorTitle->setText(tool.name);
        _operatorStack->setCurrentIndex(_editorIndexes.value(tool.id, 0));
        refreshReferencePreview();
    }
    _contentStack->setCurrentIndex(0);
}

void AlgorithmConfigPage::addTool(const QString& kind)
{
    const auto* descriptor = OperatorRegistry::find(kind);
    if (!descriptor)
        return;
    for (const auto& tool : _configuredTools)
        if (tool.kind == kind && !descriptor->allowsMultiple)
            return;
    auto tools = configuredTools();
    SchemeTool added = OperatorRegistry::makeTool(kind);
    int count = 0;
    for (const auto& tool : tools)
        if (tool.kind == kind)
            ++count;
    if (count)
        added.name += QStringLiteral(" %1").arg(count + 1);
    if (descriptor->producesAlignment)
        tools.prepend(added);
    else
        tools.append(added);
    setConfiguredTools(tools);
    _configuredToolList->setCurrentRow(descriptor->producesAlignment ? 0 : tools.size() - 1);
    Q_EMIT configuredToolsChanged(_configuredTools);
}

void AlgorithmConfigPage::removeSelectedTool()
{
    const int row = _configuredToolList->currentRow();
    if (row < 0 || row >= _configuredTools.size())
        return;
    auto tools = configuredTools();
    tools.removeAt(row);
    setConfiguredTools(tools);
    _configuredToolList->setCurrentRow(qMin(row, tools.size() - 1));
    Q_EMIT configuredToolsChanged(_configuredTools);
}

void AlgorithmConfigPage::setConfiguredTools(const QList<SchemeTool>& tools)
{
    const int oldRow = _configuredToolList->currentRow();
    const QString selectedId
        = oldRow >= 0 && oldRow < _configuredTools.size() ? _configuredTools.at(oldRow).id : QString();
    _configuredTools = tools;
    _editors.clear();
    _ruleEditors.clear();
    _editorIndexes.clear();
    while (_operatorStack->count() > 1) {
        auto* old = _operatorStack->widget(1);
        _operatorStack->removeWidget(old);
        delete old;
    }
    for (const auto& tool : tools) {
        const auto* d = OperatorRegistry::find(tool.kind);
        if (!d)
            continue;
        auto* tabs = new QTabWidget(_operatorStack);
        auto* editor = OperatorEditorRegistry::create(tool.kind, tabs);
        editor->setConfiguration(tool.parameters);
        auto* scroll = new QScrollArea(tabs);
        scroll->setWidgetResizable(true);
        scroll->setFrameShape(QFrame::NoFrame);
        scroll->setWidget(editor);
        tabs->addTab(scroll, "参数");
        auto* rules = new RuleSetEditor(d->outputs, tabs);
        rules->setConfiguration(tool.rules);
        auto* rulesScroll = new QScrollArea(tabs);
        rulesScroll->setWidgetResizable(true);
        rulesScroll->setFrameShape(QFrame::NoFrame);
        rulesScroll->setWidget(rules);
        tabs->addTab(rulesScroll, "判定条件");
        _editors.insert(tool.id, editor);
        _ruleEditors.insert(tool.id, rules);
        _editorIndexes.insert(tool.id, _operatorStack->addWidget(tabs));
    }
    {
        QSignalBlocker block(_configuredToolList);
        _configuredToolList->clear();
        for (const auto& tool : _configuredTools) {
            auto* item = new QListWidgetItem(tool.name, _configuredToolList);
            item->setSizeHint(QSize(0, 72));
        }
        int row = 0;
        for (int i = 0; i < _configuredTools.size(); ++i)
            if (_configuredTools.at(i).id == selectedId) {
                row = i;
                break;
            }
        _configuredToolList->setCurrentRow(_configuredTools.isEmpty() ? -1 : row);
    }
    _toolCount->setText(QStringLiteral("%1 个").arg(_configuredTools.size()));
    const auto hasKind = [this](const QString& kind) {
        for (const auto& tool : _configuredTools)
            if (tool.kind == kind)
                return true;
        return false;
    };
    for (auto it = _addButtons.begin(); it != _addButtons.end(); ++it) {
        it.value()->setEnabled(OperatorRegistry::find(it.key())->allowsMultiple || !hasKind(it.key()));
        it.value()->setText(
            it.value()->isEnabled() ? QStringLiteral("添加到方案") : QStringLiteral("已添加"));
    }
    showToolEditor(_configuredToolList->currentRow());
}

QList<SchemeTool> AlgorithmConfigPage::configuredTools() const
{
    auto tools = _configuredTools;
    for (auto& tool : tools) {
        if (auto* editor = _editors.value(tool.id))
            tool.parameters = editor->configuration();
        if (auto* rules = _ruleEditors.value(tool.id))
            tool.rules = rules->configuration();
    }
    return tools;
}
void AlgorithmConfigPage::refreshReferencePreview()
{
    for (auto* editor : _editors)
        editor->refreshReferencePreview();
}

void AlgorithmConfigPage::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    refreshReferencePreview();
}
