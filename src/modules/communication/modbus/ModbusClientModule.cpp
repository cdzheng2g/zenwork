#include "ModbusModules.h"
#include "infrastructure/communication/ModbusTcpOutputService.h"
#include <QObject>
#include <cmath>
CommunicationModule modbusClientModule()
{
    CommunicationModule m;
    m.id = "modbus_tcp_client";
    m.asynchronousDelivery = true;
    m.name = "Modbus TCP Client";
    m.description = "相机主动连接服务器，写入判定和检测序号。";
    m.help = "保持寄存器 0：1=OK、2=NG、3=ERROR；寄存器 1：检测序号低 16 位。";
    m.parameters
        = { textParameter("host", "服务器地址"), numberParameter("port", "服务器端口", 502, 1, 65535),
              numberParameter("unitId", "Unit ID", 1, 1, 247) };
    m.start = [](const QJsonObject& p, QString* error) {
        auto& s = ModTcpOutputService::instance();
        s.configure(p["host"].toString().trimmed(), quint16(p["port"].toInt()), p["unitId"].toInt());
        s.connectToServer();
        const bool ok = s.isConnected() || s.isConnecting();
        if (!ok && error)
            *error = "Modbus 服务器连接失败";
        return ok;
    };
    m.stop = [] { ModTcpOutputService::instance().disconnectFromServer(); };
    m.status = [] {
        auto& s = ModTcpOutputService::instance();
        return s.isConnected() ? QStringLiteral("已连接")
            : s.isConnecting() ? QStringLiteral("正在连接")
                               : QStringLiteral("未连接");
    };
    m.publish = [](const QJsonObject& result, QString* error) {
        const auto status = result["status"].toString();
        const quint16 verdict = status == "OK" ? 1 : status == "NG" ? 2 : 3;
        const auto n = result["sequence"].toDouble();
        const quint16 sequence = std::isfinite(n) && n >= 0 ? quint16(std::fmod(n, 65536.0)) : 0;
        return ModTcpOutputService::instance().publish(verdict, sequence, error,
            result["inspectionId"].toString(), result["sequenceText"].toString());
    };
    m.observe = [](QObject* owner, std::function<void()> changed, std::function<void(QString)> error) {
        auto& s = ModTcpOutputService::instance();
        QObject::connect(
            &s, &ModTcpOutputService::connectionChanged, owner, [changed, error](bool connected) {
                if (connected)
                    error({ });
                changed();
            });
        QObject::connect(&s, &ModTcpOutputService::errorOccurred, owner, error);
    };
    return m;
}
