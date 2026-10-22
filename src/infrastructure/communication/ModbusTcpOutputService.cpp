#include "ModbusTcpOutputService.h"

#include <QModbusDataUnit>
#include <QModbusDevice>
#include <QModbusReply>
#include <QModbusTcpClient>
#include <QVariant>

ModTcpOutputService::ModTcpOutputService(QObject* parent)
    : QObject(parent)
{
    _client = new QModbusTcpClient(this);

    connect(_client, &QModbusDevice::stateChanged, this, &ModTcpOutputService::onStateChanged);

    connect(_client, &QModbusDevice::errorOccurred, this, &ModTcpOutputService::onClientError);
}

ModTcpOutputService& ModTcpOutputService::instance()
{
    static ModTcpOutputService instance;
    return instance;
}

void ModTcpOutputService::configure(const QString& host, quint16 port, int unitId)
{
    _client->setConnectionParameter(QModbusDevice::NetworkAddressParameter, QVariant(host));
    _client->setConnectionParameter(QModbusDevice::NetworkPortParameter, QVariant(static_cast<int>(port)));

    _unitId = unitId;
}

void ModTcpOutputService::connectToServer()
{
    if (_client->state() != QModbusDevice::UnconnectedState)
        return;

    if (!_client->connectDevice())
        emit errorOccurred(_client->errorString());
}

void ModTcpOutputService::disconnectFromServer()
{
    _client->disconnectDevice();
}

bool ModTcpOutputService::isConnected() const
{
    return _client->state() == QModbusDevice::ConnectedState;
}

bool ModTcpOutputService::isConnecting() const
{
    return _client->state() == QModbusDevice::ConnectingState;
}

void ModTcpOutputService::onStateChanged()
{
    emit connectionChanged(isConnected());
}

void ModTcpOutputService::onClientError()
{
    emit errorOccurred(_client->errorString());
}

bool ModTcpOutputService::publish(quint16 verdict, quint16 sequence, QString* error,
    const QString& inspectionId, const QString& sequenceText)
{
    if (error)
        error->clear();

    if (!isConnected()) {
        if (error)
            *error = QStringLiteral("未连接 Modbus TCP Server");
        return false;
    }

    QModbusDataUnit data(QModbusDataUnit::HoldingRegisters, 0, 2);
    data.setValue(0, verdict);
    data.setValue(1, sequence);

    QModbusReply* reply = _client->sendWriteRequest(data, _unitId);
    if (!reply) {
        if (error)
            *error = _client->errorString();
        return false;
    }

    if (reply->isFinished()) {
        const bool success = reply->error() == QModbusDevice::NoError;
        const QString message = success ? QString() : reply->errorString();

        if (!success) {
            if (error)
                *error = message;
            emit errorOccurred(message);
        }

        emit writeFinished(success, message);
        emit deliveryFinished({inspectionId, sequenceText, "modbus_tcp_client",
            success ? DeliveryState::Confirmed : reply->error() == QModbusDevice::TimeoutError
                ? DeliveryState::TimedOut : DeliveryState::Failed, message});
        reply->deleteLater();
        return success;
    }

    reply->setProperty("inspectionId", inspectionId);
    reply->setProperty("sequenceText", sequenceText);
    connect(reply, &QModbusReply::finished, this, &ModTcpOutputService::onWriteFinished);

    return true;
}

void ModTcpOutputService::onWriteFinished()
{
    auto* reply = qobject_cast<QModbusReply*>(sender());
    if (!reply)
        return;

    const bool success = reply->error() == QModbusDevice::NoError;
    const QString message = success ? QString() : reply->errorString();

    if (!success)
        emit errorOccurred(message);

    emit writeFinished(success, message);
    emit deliveryFinished({reply->property("inspectionId").toString(),
        reply->property("sequenceText").toString(), "modbus_tcp_client",
        success ? DeliveryState::Confirmed : reply->error() == QModbusDevice::TimeoutError
            ? DeliveryState::TimedOut : DeliveryState::Failed, message});
    reply->deleteLater();
}
