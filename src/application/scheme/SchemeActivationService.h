#pragma once
#include "application/InspectionRunner.h"
#include <QObject>
#include <functional>

struct SchemeActivationBackend {
    std::function<bool(const QJsonObject&, QString*)> applyCamera;
    std::function<bool(const SchemeRuntimeSettings&, QString*)> applyCommunication;
    std::function<void()> stopCommunication;
};
class SchemeActivationService final : public QObject {
    Q_OBJECT
public:
    explicit SchemeActivationService(SchemeActivationBackend backend, QObject* parent = nullptr);
    bool activate(const InspectionRequest& scheme, QString* error = nullptr);
    void deactivate();
    bool ready() const { return _ready; }
    const InspectionRequest& active() const { return _active; }
signals:
    void changing();
    void changed();
private:
    SchemeActivationBackend _backend;
    InspectionRequest _active;
    bool _ready {false};
};
