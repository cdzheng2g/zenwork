#include "ModbusTcpServerService.h"

#include <QCoreApplication>
#include <QModbusDataUnit>
#include <QModbusTcpServer>
#include <QScopedValueRollback>
#include <QVariant>
#include <cmath>

using namespace ModbusRegisters;

// Only the application may write result registers; clients write the command register.
class CameraModbusServer final : public QModbusTcpServer {
public:
    explicit CameraModbusServer(QObject* parent)
        : QModbusTcpServer(parent)
    {
    }
    bool setLocalData(const QModbusDataUnit& data)
    {
        QScopedValueRollback<bool> guard(_localWrite, true);
        return setData(data);
    }

protected:
    bool writeData(const QModbusDataUnit& data) override
    {
        if (!_localWrite
            && (data.registerType() != QModbusDataUnit::HoldingRegisters || data.startAddress() != Trigger
                || data.valueCount() != 1))
            return false;
        return QModbusTcpServer::writeData(data);
    }

private:
    bool _localWrite { false };
};

namespace {
void put32(QVector<quint16>& values, int offset, quint32 value)
{
    values[offset] = quint16(value & 0xffffu);
    values[offset + 1] = quint16(value >> 16);
}
}

ModbusTcpServerService::ModbusTcpServerService(QObject* parent)
    : QObject(parent)
    , _server(new CameraModbusServer(this))
    , _result(ResultCount, 0)
{
    QModbusDataUnitMap map;
    map.insert(QModbusDataUnit::HoldingRegisters,
        QModbusDataUnit(QModbusDataUnit::HoldingRegisters, Trigger, ResultStart + ResultCount - Trigger));
    _server->setMap(map);
    _result[Version] = 1;
    connect(_server, &QModbusDevice::stateChanged, this, [this] { emit runningChanged(isRunning()); });
    connect(_server, &QModbusDevice::errorOccurred, this, [this](QModbusDevice::Error error) {
        if (error != QModbusDevice::NoError)
            emit errorOccurred(_server->errorString());
    });
    connect(_server, &QModbusServer::dataWritten, this,
        [this](QModbusDataUnit::RegisterType table, int address, int size) {
            if (table != QModbusDataUnit::HoldingRegisters || address > Trigger || address + size <= Trigger)
                return;
            quint16 value = 0;
            if (!_server->data(table, Trigger, &value))
                return;
            const bool high = (value & 1u) != 0;
            const bool rising = high && !_triggerHigh;
            _triggerHigh = high;
            if (rising && isRunning())
                emit triggerRequested();
        });
    syncRegisters();
}

ModbusTcpServerService& ModbusTcpServerService::instance()
{
    static auto* service = new ModbusTcpServerService(QCoreApplication::instance());
    return *service;
}

bool ModbusTcpServerService::start(quint16 port, int unitId)
{
    if (port == 0 || unitId < 1 || unitId > 247) {
        emit errorOccurred(QStringLiteral("端口须为 1–65535，Unit ID 须为 1–247"));
        return false;
    }
    if (isRunning() && _port == port && _unitId == unitId)
        return true;
    if (isRunning())
        stop();
    _port = port;
    _unitId = unitId;
    _server->setServerAddress(unitId);
    _server->setConnectionParameter(QModbusDevice::NetworkAddressParameter, QStringLiteral("0.0.0.0"));
    _server->setConnectionParameter(QModbusDevice::NetworkPortParameter, port);
    _server->setLocalData(
        QModbusDataUnit(QModbusDataUnit::HoldingRegisters, Trigger, QVector<quint16> { 0 }));
    _triggerHigh = false;
    syncRegisters();
    if (!_server->connectDevice()) {
        emit errorOccurred(_server->errorString());
        return false;
    }
    return true;
}

void ModbusTcpServerService::stop()
{
    _server->disconnectDevice();
}
bool ModbusTcpServerService::isRunning() const
{
    return _server->state() == QModbusDevice::ConnectedState;
}

void ModbusTcpServerService::syncRegisters()
{
    _server->setLocalData(QModbusDataUnit(QModbusDataUnit::HoldingRegisters, ResultStart, _result));
}

void ModbusTcpServerService::setExecutionState(bool ready, bool busy)
{
    _result[State] = busy ? Busy : ready ? Ready : NotReady;
    syncRegisters();
}

void ModbusTcpServerService::respondToTrigger(TriggerResponse response)
{
    _result[TriggerReply] = response;
    if (response == BusyRejected || response == NotReadyRejected)
        put32(_result, RejectedLow, ++_rejected);
    syncRegisters();
}

bool ModbusTcpServerService::publish(const QJsonObject& result, QString* error)
{
    if (!isRunning()) {
        if (error)
            *error = QStringLiteral("Modbus TCP Server 未启动");
        return false;
    }
    const QString status = result.value(QStringLiteral("status")).toString();
    _result[Verdict] = status == QStringLiteral("OK") ? 1 : status == QStringLiteral("NG") ? 2 : 3;
    const double sequence = result.value(QStringLiteral("sequence")).toDouble();
    const double elapsed = result.value(QStringLiteral("elapsedMs")).toDouble();
    // Unsigned 32-bit values use the low register first.
    put32(_result, SequenceLow,
        std::isfinite(sequence) && sequence >= 0 ? quint32(std::fmod(sequence, 4294967296.0)) : 0);
    put32(_result, ElapsedLow,
        std::isfinite(elapsed) && elapsed >= 0 ? quint32(qMin(elapsed, 4294967295.0)) : 0);
    put32(_result, CompletedLow, ++_completed);
    syncRegisters();
    return true;
}
