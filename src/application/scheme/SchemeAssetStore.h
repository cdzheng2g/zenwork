#pragma once
#include "SchemePackage.h"
namespace SchemeAssetStore {
QJsonObject encode(const QByteArray& data);
QByteArray decode(const QJsonObject& object);
QJsonObject prepareForSave(SchemePackage& package, bool includeModels);
void loadInto(SchemePackage& package, const QJsonObject& assets, const QString& mode, bool legacy);
bool install(SchemePackage& package, const QString& directory, QString* error);
}
