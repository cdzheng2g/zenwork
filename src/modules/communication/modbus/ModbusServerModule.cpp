#include "ModbusModules.h"
#include "infrastructure/communication/ModbusTcpServerService.h"
#include <QObject>
CommunicationModule modbusServerModule()
{
    CommunicationModule m;
    m.id = "modbus_tcp_server";
    m.name = "Modbus TCP Server";
    m.listener = true;
    m.triggerInput = true;
    m.description = "电脑或 PLC 连接相机，写触发指令、读检测结果。";
    m.help = "保持寄存器（十进制、从 0 开始）\n256：bit 0 从 0→1 "
             "触发\n768：0=未就绪，1=就绪，2=检测中\n769：0=无结果，1=OK，2=NG，3="
             "ERROR\n770–771：检测序号；772–773：耗时 "
             "ms\n774–775：完成次数；776–777：拒绝次数\n778：1=接受，2=忙，3=未就绪\n779：协议版本；32 "
             "位数值低字在前。";
    m.parameters = { numberParameter("port", "监听端口", 1502, 1, 65535),
        numberParameter("unitId", "Unit ID", 1, 1, 247) };
    m.start = [](const QJsonObject& p, QString* error) {
        const bool ok
            = ModbusTcpServerService::instance().start(quint16(p["port"].toInt()), p["unitId"].toInt());
        if (!ok && error)
            *error = "Modbus 监听失败，请检查端口占用";
        return ok;
    };
    m.stop = [] { ModbusTcpServerService::instance().stop(); };
    m.status = [] {
        auto& s = ModbusTcpServerService::instance();
        return s.isRunning() ? QStringLiteral("监听 %1 · Unit ID %2").arg(s.port()).arg(s.unitId())
                             : QStringLiteral("监听未启动");
    };
    m.publish = [](const QJsonObject& result, QString* error) {
        return ModbusTcpServerService::instance().publish(result, error);
    };
    m.observe = [](QObject* owner, std::function<void()> changed, std::function<void(QString)> error) {
        auto& s = ModbusTcpServerService::instance();
        QObject::connect(&s, &ModbusTcpServerService::runningChanged, owner, [changed] { changed(); });
        QObject::connect(&s, &ModbusTcpServerService::errorOccurred, owner, error);
    };
    return m;
}
