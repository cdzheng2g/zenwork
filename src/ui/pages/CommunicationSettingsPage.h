#pragma once
#include "application/scheme/SchemeCatalog.h"
#include <QFrame>
#include <QMap>

class QCheckBox;
class QLabel;
class CommunicationParameterPanel;

class CommunicationSettingsPage final : public QFrame {
    Q_OBJECT
public:
    explicit CommunicationSettingsPage(QWidget* parent = nullptr);
    void setSettings(const SchemeRuntimeSettings& settings);
    QJsonObject configuration() const;
    QStringList outputChannels() const;
    void enableOutput(const QString& id);
    void setModbusTriggerRequired(bool required);

private:
    void refreshStatus();
    QMap<QString, QCheckBox*> _outputs;
    QMap<QString, CommunicationParameterPanel*> _panels;
    QMap<QString, QLabel*> _statuses;
    QJsonObject _extraConfiguration;
    bool _modbusTriggerRequired { false };
};
