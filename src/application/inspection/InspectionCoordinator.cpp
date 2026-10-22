#include "InspectionCoordinator.h"
InspectionCoordinator::InspectionCoordinator(InspectionService& inspection,
    SchemeActivationBackend backend, QObject* parent)
    : QObject(parent), _inspection(inspection), _activation(std::move(backend)), _trigger()
{
    connect(&_activation, &SchemeActivationService::changing, this, [this] {
        _trigger.stop();
        _inspection.invalidate();
        _inspection.clearFrame();
    });
    connect(&_activation, &SchemeActivationService::changed, this, &InspectionCoordinator::updateTrigger);
    connect(&_trigger, &InternalTrigger::triggered, this, [this] { submit(TriggerSource::Internal); });
    connect(&_inspection, &InspectionService::finished, this, [this] {
        _trigger.finishExecution();
        emit stateChanged();
    });
    connect(&_inspection, &InspectionService::faulted, this, [this] { _trigger.stop(); emit stateChanged(); });
}
bool InspectionCoordinator::activate(const InspectionRequest& scheme, QString* error)
{
    return _activation.activate(scheme, error);
}
void InspectionCoordinator::deactivate() { _activation.deactivate(); }
void InspectionCoordinator::setMode(InspectionMode mode)
{
    const bool changed = mode.editing != _mode.editing || mode.fileBusy != _mode.fileBusy || mode.paused != _mode.paused;
    _mode = mode;
    if (changed) _inspection.invalidate();
    updateTrigger();
}
void InspectionCoordinator::setPaused(bool value) { auto mode = _mode; mode.paused = value; setMode(mode); }
void InspectionCoordinator::setEditing(bool value) { auto mode = _mode; mode.editing = value; setMode(mode); }
void InspectionCoordinator::setFileBusy(bool value) { auto mode = _mode; mode.fileBusy = value; setMode(mode); }
InspectionGate InspectionCoordinator::gate() const
{
    const auto& active = _activation.active();
    return {!active.scheme.isEmpty(), !active.tools.isEmpty(), _mode.editing, _mode.fileBusy, _mode.paused,
        active.settings.internalTriggerEnabled, active.settings.modbusTriggerEnabled,
        _activation.ready(), _activation.ready()};
}
bool InspectionCoordinator::modbusReady() const
{
    return !_inspection.requiresRecovery()
        && triggerDecision(TriggerSource::Modbus, gate(), false, _inspection.hasFreshFrame()) == TriggerDecision::Accepted;
}
void InspectionCoordinator::updateTrigger()
{
    _trigger.stop();
    const auto& settings = _activation.active().settings;
    if (_activation.ready() && !_mode.editing && !_mode.fileBusy && !_mode.paused
        && settings.internalTriggerEnabled && !_inspection.requiresRecovery())
        _trigger.start(settings.triggerIntervalMs);
    emit stateChanged();
}
TriggerDecision InspectionCoordinator::submit(TriggerSource source, const QString& toolId)
{
    auto decision = _inspection.admission(source, gate());
    if (decision == TriggerDecision::Accepted && source != TriggerSource::Internal
        && !_trigger.tryBeginExecution()) decision = TriggerDecision::Busy;
    if (decision == TriggerDecision::Accepted)
        decision = _inspection.start(_activation.active(), source, gate(), toolId, _trigger.skippedTickCount());
    if (decision != TriggerDecision::Accepted && !_inspection.isRunning()) _trigger.finishExecution();
    emit submission(decision);
    emit stateChanged();
    return decision;
}
void InspectionCoordinator::shutdown() { _trigger.stop(); _inspection.shutdown(); }
