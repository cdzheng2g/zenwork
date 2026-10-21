#include "CommunicationSettingsPage.h"
#include "ui/communication/CommunicationPanelRegistry.h"
#include "application/communication/ResultOutputDispatcher.h"
#include "application/communication/SchemeCommunicationRuntime.h"
#include <QCheckBox>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QTimer>
#include <QVBoxLayout>

CommunicationSettingsPage::CommunicationSettingsPage(QWidget* parent)
    : QFrame(parent)
{
    setObjectName("configPanel");
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(16, 16, 16, 16);
    root->setSpacing(12);
    auto* title = new QLabel(QStringLiteral("通信与输出"), this);
    title->setObjectName("sectionTitle");
    root->addWidget(title);
    auto* scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    auto* content = new QWidget(scroll);
    auto* cards = new QVBoxLayout(content);
    cards->setContentsMargins(0, 0, 6, 0);
    cards->setSpacing(12);
    for (const auto& channel : ResultOutputDispatcher::channels()) {
        auto* card = new QFrame(content);
        card->setObjectName("communicationCard");
        card->setStyleSheet(
            "QFrame#communicationCard {background:white;border:1px solid #dce2e8;border-radius:5px;}");
        auto* layout = new QVBoxLayout(card);
        layout->setContentsMargins(16, 12, 16, 12);
        layout->setSpacing(8);
        auto* row = new QHBoxLayout;
        auto* check = new QCheckBox(channel.name, card);
        check->setObjectName(channel.id + "Output");
        check->setStyleSheet("font-size:14px;font-weight:600;");
        auto* status = new QLabel(card);
        status->setObjectName("muted");
        status->setWordWrap(true);
        row->addWidget(check);
        row->addStretch(1);
        row->addWidget(status);
        layout->addLayout(row);
        auto* panel = createCommunicationPanel(channel.id, card);
        if (!panel) {
            delete card;
            continue;
        }
        panel->setObjectName(channel.id + "Parameters");
        layout->addWidget(panel);
        cards->addWidget(card);
        _outputs.insert(channel.id, check);
        _panels.insert(channel.id, panel);
        _statuses.insert(channel.id, status);
        connect(check, &QCheckBox::toggled, this, [this] { refreshStatus(); });
    }
    cards->addStretch(1);
    scroll->setWidget(content);
    root->addWidget(scroll, 1);
    auto* automatic = new QLabel(QStringLiteral("通信参数随方案保存，保存后自动生效。"), this);
    automatic->setObjectName("muted");
    root->addWidget(automatic);
    connect(&SchemeCommunicationRuntime::instance(), &SchemeCommunicationRuntime::statusChanged, this,
        &CommunicationSettingsPage::refreshStatus);
    auto* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &CommunicationSettingsPage::refreshStatus);
    timer->start(500);
    setSettings(SchemeRuntimeSettings { });
}

void CommunicationSettingsPage::setSettings(const SchemeRuntimeSettings& settings)
{
    const auto config = normalizedCommunicationSettings(settings.communication);
    _extraConfiguration = config;
    _modbusTriggerRequired = settings.modbusTriggerEnabled;
    for (auto i = _panels.begin(); i != _panels.end(); ++i) {
        i.value()->setConfiguration(config.value(i.key()).toObject());
        _outputs.value(i.key())->setChecked(settings.outputChannels.contains(i.key()));
    }
    refreshStatus();
}

QJsonObject CommunicationSettingsPage::configuration() const
{
    auto result = _extraConfiguration;
    for (auto i = _panels.begin(); i != _panels.end(); ++i)
        result[i.key()] = i.value()->configuration();
    return result;
}

QStringList CommunicationSettingsPage::outputChannels() const
{
    QStringList result;
    for (auto i = _outputs.begin(); i != _outputs.end(); ++i)
        if (i.value()->isChecked())
            result.append(i.key());
    return result;
}

void CommunicationSettingsPage::enableOutput(const QString& id)
{
    if (_outputs.contains(id))
        _outputs.value(id)->setChecked(true);
}

void CommunicationSettingsPage::setModbusTriggerRequired(bool required)
{
    _modbusTriggerRequired = required;
    refreshStatus();
}

void CommunicationSettingsPage::refreshStatus()
{
    auto& runtime = SchemeCommunicationRuntime::instance();
    for (auto i = _panels.begin(); i != _panels.end(); ++i) {
        const bool output = _outputs.value(i.key())->isChecked();
        const bool required = output || (i.key() == "modbus_tcp_server" && _modbusTriggerRequired);
        i.value()->setVisible(required);
        const bool changed = required != runtime.usesChannel(i.key())
            || (required && i.value()->configuration() != runtime.configuration(i.key()));
        QString text = changed ? QStringLiteral("待保存") : runtime.status(i.key());
        if (required && !output)
            text.prepend(QStringLiteral("仅用于触发 · "));
        _statuses.value(i.key())->setText(text);
    }
}
