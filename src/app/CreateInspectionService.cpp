#include "CreateInspectionService.h"
#include "application/inspection/InspectionService.h"
#include "infrastructure/persistence/InspectionArchive.h"
#include "application/communication/ResultOutputDispatcher.h"
#include "infrastructure/inference/InspectionProcess.h"
#include <QCoreApplication>
#include <QDebug>
#include "infrastructure/persistence/ArchiveQueue.h"
#include "application/communication/ResultDeliveryTracker.h"
InspectionService* createInspectionService(QObject* parent)
{
    bool valid = false;
    const int configured = qEnvironmentVariableIntValue("HANS_INSPECTION_TIMEOUT_MS", &valid);
    const int timeoutMs = valid && configured >= 1 && configured <= 600000 ? configured : 60000;
    auto* archive = new ArchiveQueue(&InspectionArchive::appendLine);
    auto* service = new InspectionService(
        &runIsolatedInspection,
        [archive](const QStringList& channels, const QJsonObject& result) {
            auto errors = ResultOutputDispatcher::publish(channels, result);
            QString archiveError;
            auto& tracker = ResultDeliveryTracker::instance();
            if (!tracker.begin("archive", result, &archiveError))
                errors << "结果保存失败：" + archiveError;
            else if (!archive->enqueue(result, &archiveError)) {
                tracker.finish({result["inspectionId"].toString(), result["sequenceText"].toString(),
                    "archive", DeliveryState::Failed, archiveError});
                errors << "结果保存失败：" + archiveError;
            }
            return errors;
        },
        parent, timeoutMs);
    archive->setParent(service);
    auto& tracker = ResultDeliveryTracker::instance();
    QObject::connect(archive, &ArchiveQueue::completed, &tracker, &ResultDeliveryTracker::finish);
    QObject::connect(&tracker, &ResultDeliveryTracker::changed, service,
        [service, archive](const DeliveryEvent& event) {
            service->recordDelivery(event);
            QString error;
            if (!archive->enqueue(event.toJson(), &error, false))
                qWarning().noquote() << "交付状态未保存:" << event.inspectionId << event.channel << error;
        });
    QObject::connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit,
        service, [service, archive] { service->shutdown(); archive->shutdown(); });
    return service;
}
