#pragma once

#include <QObject>
#include <QJsonObject>
#include <QVector>
#include "ModbusRegisterMap.h"

class CameraModbusServer;

class ModbusTcpServerService final : public QObject {
    Q_OBJECT
public:
    explicit ModbusTcpServerService(QObject* parent = nullptr);
    static ModbusTcpServerService& instance();
    bool start(quint16 port = 1502, int unitId = 1);
    void stop();
    bool isRunning() const;
    quint16 port() const
    {
        return _port;
    }
    int unitId() const
    {
        return _unitId;
    }
    void setExecutionState(bool ready, bool busy);
    void respondToTrigger(ModbusRegisters::TriggerResponse response);
    bool publish(const QJsonObject& result, QString* error = nullptr);

signals:
    void runningChanged(bool running);
    void errorOccurred(const QString& error);
    void triggerRequested();

private:
    void syncRegisters();
    CameraModbusServer* _server;
    QVector<quint16> _result;
    quint16 _port { 1502 };
    int _unitId { 1 };
    bool _triggerHigh { false };
    quint32 _completed { 0 };
    quint32 _rejected { 0 };
};
