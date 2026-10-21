#include "CreateInspectionService.h"
#include "application/inspection/InspectionService.h"
#include "infrastructure/persistence/InspectionArchive.h"
#include "application/communication/ResultOutputDispatcher.h"
InspectionService* createInspectionService(QObject* parent)
{
    return new InspectionService(
        &runInspection,
        [](const QStringList& channels, const QJsonObject& result) {
            auto errors = ResultOutputDispatcher::publish(channels, result);
            const auto archiveError = InspectionArchive::append(result);
            if (!archiveError.isEmpty())
                errors << "结果保存失败：" + archiveError;
            return errors;
        },
        parent);
}
