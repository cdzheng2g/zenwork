#pragma once

#include "CommunicationSettings.h"
#include "contracts/SchemeConfiguration.h"
#include <QMap>
#include <QString>
#include <QStringList>
#include <QJsonObject>

// 仅保存工程人员在主控中可见的真实方案。草稿不写入这个目录，因此空系统
// 不会出现虚构的默认 OCR 方案。
class SchemeCatalog final {
public:
    explicit SchemeCatalog(QString storageDirectory = { });

    bool load(QString* error = nullptr);
    bool save(const QString& schemeName, QString* error = nullptr);
    bool save(const QString& schemeName, const SchemeRuntimeSettings& settings, QString* error = nullptr);
    bool setTools(const QString& schemeName, const QList<SchemeTool>& tools, QString* error = nullptr);
    bool saveComplete(const QString& name, const SchemeRuntimeSettings& settings,
        const QList<SchemeTool>& tools, QString* error = nullptr);
    QString portablePath(const QString& name) const;
    bool remove(const QString& schemeName, QString* error = nullptr);

    QStringList schemeNames() const;
    QString currentSchemeName() const;
    bool select(const QString& schemeName);
    SchemeRuntimeSettings settings(const QString& schemeName) const;
    QList<SchemeTool> tools(const QString& schemeName) const;

private:
    QString catalogPath() const;
    bool persist(QString* error) const;

    QString _storageDirectory;
    QStringList _schemeNames;
    QString _currentSchemeName;
    QMap<QString, SchemeRuntimeSettings> _settings;
    QMap<QString, QList<SchemeTool>> _tools;
};
