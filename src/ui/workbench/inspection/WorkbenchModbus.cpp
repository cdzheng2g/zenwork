#include "ui/workbench/WorkbenchWidget.h"
#include "application/InternalTrigger.h"
#include "infrastructure/communication/ModbusTcpServerService.h"
bool WorkbenchWidget::modbusTriggerReady() const
{
    return triggerDecision(TriggerSource::Modbus, inspectionGate(), false, _inspection->hasFreshFrame())
        == TriggerDecision::Accepted;
}
void WorkbenchWidget::refreshModbusState()
{
    ModbusTcpServerService::instance().setExecutionState(modbusTriggerReady(), _inspection->isRunning());
}
void WorkbenchWidget::handleModbusTrigger()
{
    auto& service = ModbusTcpServerService::instance();
    const auto decision = _inspection->admission(TriggerSource::Modbus, inspectionGate());
    if (decision == TriggerDecision::Busy) {
        service.respondToTrigger(ModbusRegisters::BusyRejected);
        return;
    }
    if (decision == TriggerDecision::NotReady) {
        service.respondToTrigger(ModbusRegisters::NotReadyRejected);
        return;
    }
    if (!_internalTrigger->tryBeginExecution()) {
        service.respondToTrigger(ModbusRegisters::BusyRejected);
        return;
    }
    service.respondToTrigger(executeInspection(TriggerSource::Modbus) ? ModbusRegisters::Accepted
                                                                      : ModbusRegisters::NotReadyRejected);
}
