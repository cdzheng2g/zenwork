#include "SchemeActivationService.h"
#include "application/scheme/SchemeCodec.h"
#include <exception>
SchemeActivationService::SchemeActivationService(SchemeActivationBackend backend, QObject* parent)
    : QObject(parent), _backend(std::move(backend)) {}
bool SchemeActivationService::activate(const InspectionRequest& scheme, QString* error)
{
    _ready = false;
    emit changing();
    QString failure;
    try {
    failure = SchemeCodec::validate(scheme.settings, scheme.tools);
    if (scheme.scheme.isEmpty()) failure = QStringLiteral("方案名称为空");
    if (failure.isEmpty() && _backend.applyCamera
        && !_backend.applyCamera(scheme.settings.camera, &failure) && failure.isEmpty())
        failure = QStringLiteral("相机参数未完全生效");
    if (failure.isEmpty() && _backend.applyCommunication
        && !_backend.applyCommunication(scheme.settings, &failure) && failure.isEmpty())
        failure = QStringLiteral("通信配置未生效");
    } catch (const std::exception& exception) {
        failure = QString::fromUtf8(exception.what());
    } catch (...) { failure = QStringLiteral("方案激活异常"); }
    if (failure.isEmpty()) {
        _active = scheme;
        _ready = true;
    } else if (_backend.stopCommunication) _backend.stopCommunication();
    if (error) *error = failure;
    emit changed();
    return _ready;
}
void SchemeActivationService::deactivate()
{
    emit changing();
    _ready = false;
    _active = {};
    if (_backend.stopCommunication) _backend.stopCommunication();
    emit changed();
}
