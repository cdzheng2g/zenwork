#pragma once
#include "SchemeCatalog.h"
#include "contracts/ReferenceImage.h"

// A scheme keeps its reference image. OCR resources are addressed by SHA-256;
// a full migration export also embeds their bytes for an unprovisioned device.
struct SchemePackage {
    QString name;
    SchemeRuntimeSettings settings;
    QList<SchemeTool> tools;
    ReferenceImage reference;
    QMap<QString, QByteArray> resources;
    QMap<QString, QString> resourceHashes;

    bool save(const QString& path, QString* error = nullptr, bool includeModels = false) const;
    static bool load(const QString& path, SchemePackage& package, QString* error = nullptr);
    bool installResources(
        const QString& directory, SchemeRuntimeSettings& installed, QString* error = nullptr);
};
