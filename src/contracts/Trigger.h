#pragma once
#include <QString>
enum class TriggerSource { Internal, Manual, Modbus };
enum class TriggerDecision { Accepted, Busy, NotReady };
struct InspectionGate {
    bool hasScheme { false }, hasTools { false }, editing { false }, fileBusy { false }, paused { false };
    bool internalEnabled { false }, modbusEnabled { false }, communicationReady { false };
    bool activationReady { true };
};
inline TriggerDecision triggerDecision(
    TriggerSource source, const InspectionGate& gate, bool busy, bool freshFrame)
{
    if (busy)
        return TriggerDecision::Busy;
    if (!gate.activationReady || !gate.hasScheme || !gate.hasTools || gate.editing || gate.fileBusy || !freshFrame)
        return TriggerDecision::NotReady;
    if (source != TriggerSource::Manual && gate.paused)
        return TriggerDecision::NotReady;
    if (source == TriggerSource::Internal && !gate.internalEnabled)
        return TriggerDecision::NotReady;
    if (source == TriggerSource::Modbus && (!gate.modbusEnabled || !gate.communicationReady))
        return TriggerDecision::NotReady;
    return TriggerDecision::Accepted;
}
