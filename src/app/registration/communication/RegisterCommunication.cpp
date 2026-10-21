#include "modules/communication/tcp/TcpModule.h"
#include "modules/communication/modbus/ModbusModules.h"
const QList<CommunicationModule>& communicationModules()
{
    static const QList<CommunicationModule> modules { tcpModule(), modbusClientModule(),
        modbusServerModule() };
    return modules;
}
