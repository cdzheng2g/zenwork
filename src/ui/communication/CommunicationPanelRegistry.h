#pragma once
#include <QWidget>
#include <QJsonObject>
#include <QMap>

class ParameterForm;

class CommunicationParameterPanel final : public QWidget {
public:
    explicit CommunicationParameterPanel(const QString& channelId, QWidget* parent = nullptr);
    QJsonObject configuration() const;
    void setConfiguration(const QJsonObject& configuration);

private:
    ParameterForm* _form { nullptr };
};

CommunicationParameterPanel* createCommunicationPanel(const QString& channelId, QWidget* parent);
