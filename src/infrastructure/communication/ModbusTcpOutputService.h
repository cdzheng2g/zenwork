#pragma once

#include <QObject>
#include <QString>
#include <QtGlobal>
#include "contracts/DeliveryEvent.h"

class QModbusTcpClient;

class ModTcpOutputService final : public QObject {
    Q_OBJECT

public:
    explicit ModTcpOutputService(QObject* parent = nullptr);

    void configure(const QString& host, quint16 port, int unitId);
    void connectToServer();
    void disconnectFromServer();

    bool isConnected() const;
    bool publish(quint16 verdict, quint16 sequence, QString* error = nullptr,
        const QString& inspectionId = {}, const QString& sequenceText = {});
    bool isConnecting() const;
    static ModTcpOutputService& instance();
signals:
    void connectionChanged(bool connected);
    void errorOccurred(const QString& error);
    void writeFinished(bool success, const QString& message);
    void deliveryFinished(const DeliveryEvent& event);

private:
    void onStateChanged();
    void onClientError();
    void onWriteFinished();

    QModbusTcpClient* _client { nullptr };
    int _unitId { 1 };
};
