#include "CreateInspectionService.h"
#include "application/inspection/InspectionService.h"
#include "infrastructure/persistence/InspectionArchive.h"
#include "application/communication/ResultOutputDispatcher.h"
InspectionService* createInspectionService(QObject* parent)
{
    bool valid = false;
    const int configured = qEnvironmentVariableIntValue("HANS_INSPECTION_TIMEOUT_MS", &valid);
    const int timeoutMs = valid && configured >= 1 && configured <= 600000 ? configured : 60000;
    return new InspectionService(
        &runInspection,
        [](const QStringList& channels, const QJsonObject& result) {
            auto errors = ResultOutputDispatcher::publish(channels, result);
            const auto archiveError = InspectionArchive::append(result);
            if (!archiveError.isEmpty())
                errors << "结果保存失败：" + archiveError;
            return errors;
        },
        parent, timeoutMs);
}
