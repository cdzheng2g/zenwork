#pragma once
#include "InspectionService.h"
#include "application/InternalTrigger.h"
#include "application/scheme/SchemeActivationService.h"

struct InspectionMode { bool editing {false}, fileBusy {false}, paused {false}; };
class InspectionCoordinator final : public QObject {
    Q_OBJECT
public:
    InspectionCoordinator(InspectionService& inspection, SchemeActivationBackend backend,
        QObject* parent = nullptr);
    bool activate(const InspectionRequest& scheme, QString* error = nullptr);
    void deactivate();
    void setMode(InspectionMode mode);
    void setPaused(bool paused);
    void setEditing(bool editing);
    void setFileBusy(bool busy);
    TriggerDecision submit(TriggerSource source, const QString& toolId = {});
    InspectionGate gate() const;
    bool modbusReady() const;
    void shutdown();
signals:
    void stateChanged();
    void submission(TriggerDecision decision);
private:
    void updateTrigger();
    InspectionService& _inspection;
    SchemeActivationService _activation;
    InternalTrigger _trigger;
    InspectionMode _mode;
};
