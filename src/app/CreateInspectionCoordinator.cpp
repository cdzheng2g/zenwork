#include "CreateInspectionCoordinator.h"
#include "application/inspection/InspectionCoordinator.h"
#include "application/communication/SchemeCommunicationRuntime.h"
#include "infrastructure/camera/CameraController.h"
#include "infrastructure/camera/CameraParamsJson.h"
#include "infrastructure/camera/CameraStreamService.h"
#include "infrastructure/reference/ReferenceImageManager.h"
#include "infrastructure/communication/ModbusTcpServerService.h"
#include <QTimer>
InspectionCoordinator* createInspectionCoordinator(InspectionService& service, QObject* parent)
{
    auto* coordinator = new InspectionCoordinator(service, {
        [](const QJsonObject& value, QString* error) {
            if (value.isEmpty()) return true;
            const bool ok = CameraController::instance().applyParams(cameraParamsFromJson(value));
            if (!ok && error) *error = QStringLiteral("方案相机参数未完全应用");
            return ok;
        },
        [](const SchemeRuntimeSettings& settings, QString* error) {
            return SchemeCommunicationRuntime::instance().apply(settings, error);
        },
        [] { SchemeCommunicationRuntime::instance().stopAll(); }
    }, parent);
    auto refresh = [coordinator, &service] {
        ModbusTcpServerService::instance().setExecutionState(coordinator->modbusReady(), service.isRunning());
    };
    QObject::connect(coordinator, &InspectionCoordinator::stateChanged, coordinator, refresh);
    auto* timer = new QTimer(coordinator);
    QObject::connect(timer, &QTimer::timeout, coordinator, refresh);
    timer->start(200);
    QObject::connect(&ModbusTcpServerService::instance(), &ModbusTcpServerService::triggerRequested,
        coordinator, [coordinator] {
            const auto decision = coordinator->submit(TriggerSource::Modbus);
            ModbusTcpServerService::instance().respondToTrigger(decision == TriggerDecision::Accepted
                ? ModbusRegisters::Accepted : decision == TriggerDecision::Busy
                    ? ModbusRegisters::BusyRejected : ModbusRegisters::NotReadyRejected);
        });
    auto& camera = CameraStreamService::instance();
    QObject::connect(&camera, &CameraStreamService::frameAvailable, &service,
        [&service](const FrameEnvelope& frame) { service.updateFrame(frame); });
    QObject::connect(&camera, &CameraStreamService::cameraStateChanged, &service,
        [&service](bool ready) { if (!ready) service.clearFrame(); });
    camera.retainForApplicationLifetime();
    return coordinator;
}
bool activateCatalogScheme(InspectionCoordinator& coordinator, const SchemeCatalog& catalog, QString* error)
{
    InspectionRequest request;
    request.scheme = catalog.currentSchemeName();
    if (request.scheme.isEmpty()) { coordinator.deactivate(); return true; }
    request.settings = catalog.settings(request.scheme);
    request.tools = catalog.tools(request.scheme);
    if (const auto* reference = ReferenceImageManager::instance().referenceImage(request.settings.referenceModel))
        request.reference = *reference;
    return coordinator.activate(request, error);
}
