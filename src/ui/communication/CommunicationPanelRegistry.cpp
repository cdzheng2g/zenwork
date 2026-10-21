#include "CommunicationPanelRegistry.h"
#include "application/scheme/CommunicationSettings.h"
#include "ui/components/ParameterForm.h"
#include "contracts/CommunicationModule.h"
#include <QFormLayout>
#include <QLabel>
#include <QToolButton>
#include <QVBoxLayout>
#include <QNetworkInterface>

CommunicationParameterPanel::CommunicationParameterPanel(const QString& channelId, QWidget* parent)
    : QWidget(parent)
{
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(0, 4, 0, 0);
    root->setSpacing(10);
    const auto* module = communicationModule(channelId);
    _form = new ParameterForm(module->parameters, this);
    root->addWidget(_form);
    if (module->listener) {
        QStringList addresses;
        for (const auto& iface : QNetworkInterface::allInterfaces()) {
            if (!(iface.flags() & QNetworkInterface::IsUp) || (iface.flags() & QNetworkInterface::IsLoopBack))
                continue;
            for (const auto& entry : iface.addressEntries())
                if (entry.ip().protocol() == QAbstractSocket::IPv4Protocol)
                    addresses << entry.ip().toString();
        }
        auto* local = new QLabel(QStringLiteral("本机地址：") + addresses.join(QStringLiteral(" / ")), this);
        local->setObjectName("muted");
        local->setWordWrap(true);
        root->addWidget(local);
    }
    const QString description = module->description;
    auto* hint = new QLabel(description, this);
    hint->setObjectName("muted");
    hint->setWordWrap(true);
    root->addWidget(hint);
    if (!module->help.isEmpty()) {
        auto* details = new QLabel(module->help, this);
        details->setWordWrap(true);
        details->setObjectName("muted");
        details->hide();
        auto* expand = new QToolButton(this);
        expand->setText(QStringLiteral("寄存器说明"));
        expand->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        expand->setArrowType(Qt::RightArrow);
        expand->setCheckable(true);
        expand->setAutoRaise(true);
        connect(expand, &QToolButton::toggled, details, [expand, details](bool shown) {
            details->setVisible(shown);
            expand->setArrowType(shown ? Qt::DownArrow : Qt::RightArrow);
        });
        root->addWidget(expand, 0, Qt::AlignLeft);
        root->addWidget(details);
    }
    setConfiguration(defaultCommunicationSettings().value(channelId).toObject());
}

QJsonObject CommunicationParameterPanel::configuration() const
{
    return _form->configuration();
}
void CommunicationParameterPanel::setConfiguration(const QJsonObject& config)
{
    _form->setConfiguration(config);
}

CommunicationParameterPanel* createCommunicationPanel(const QString& channelId, QWidget* parent)
{
    if (!defaultCommunicationSettings().contains(channelId))
        return nullptr;
    return new CommunicationParameterPanel(channelId, parent);
}
