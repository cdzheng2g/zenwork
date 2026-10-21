#include "SchemeCatalog.h"
#include "SchemeCodec.h"
#include "infrastructure/persistence/YamlDocument.h"
#include <QCryptographicHash>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QStandardPaths>

namespace {
bool validName(const QString& name)
{
    return !name.trimmed().isEmpty() && name.size() <= 64 && !name.contains('/') && !name.contains('\\')
        && !name.contains(QChar::Null);
}
bool fail(QString* error, const QString& message)
{
    if (error)
        *error = message;
    return false;
}
}
SchemeCatalog::SchemeCatalog(QString directory)
    : _storageDirectory(std::move(directory))
{
    if (_storageDirectory.isEmpty())
        _storageDirectory = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/schemes";
}
bool SchemeCatalog::load(QString* error)
{
    try {
        QJsonObject root;
        if (QFile::exists(catalogPath())) {
            if (!YamlDocument::read(catalogPath(), root, error))
                return false;
        } else {
            QFile file(_storageDirectory + "/catalog.json");
            if (!file.exists())
                return true;
            if (!file.open(QIODevice::ReadOnly))
                return fail(error, file.errorString());
            QJsonParseError parse;
            const auto doc = QJsonDocument::fromJson(file.readAll(), &parse);
            if (parse.error != QJsonParseError::NoError || !doc.isObject())
                return fail(error, "旧方案目录格式无效");
            root = doc.object();
        }
        const int version = root["formatVersion"].toInt();
        if (!root["formatVersion"].isDouble() || root["formatVersion"].toDouble() != version
            || (version != 1 && version != 2))
            return fail(error, "不支持的方案目录版本");
        if (!root["schemes"].isArray())
            return fail(error, "方案目录缺少方案列表");
        SchemeCatalog loaded(_storageDirectory);
        for (const auto& v : root["schemes"].toArray()) {
            const auto item = v.toObject();
            const auto name = item["name"].toString().trimmed();
            if (!validName(name) || loaded._schemeNames.contains(name))
                return fail(error, "方案名称无效或重复");
            if (!item["tools"].isArray())
                return fail(error, "方案缺少算子列表");
            auto settings = SchemeCodec::decodeSettings(item, true);
            // Version 1 prioritized Modbus when both legacy flags were set.
            if (version == 1 && settings.modbusTriggerEnabled)
                settings.internalTriggerEnabled = false;
            const auto tools = SchemeCodec::decodeTools(item["tools"].toArray(), item["ocr"].toObject());
            const auto invalid = SchemeCodec::validate(settings, tools);
            if (!invalid.isEmpty())
                return fail(error, name + "：" + invalid);
            loaded._schemeNames.append(name);
            loaded._settings[name] = settings;
            loaded._tools[name] = tools;
        }
        const auto current = root["currentScheme"].toString();
        loaded._currentSchemeName
            = loaded._schemeNames.contains(current) ? current : loaded._schemeNames.value(0);
        *this = std::move(loaded);
        return true;
    } catch (const std::exception& e) {
        return fail(error, QString::fromUtf8(e.what()));
    }
}
bool SchemeCatalog::save(const QString& name, QString* error)
{
    return save(name, settings(name), error);
}
bool SchemeCatalog::save(const QString& name, const SchemeRuntimeSettings& settings, QString* error)
{
    return saveComplete(name, settings, tools(name), error);
}
bool SchemeCatalog::setTools(const QString& name, const QList<SchemeTool>& tools, QString* error)
{
    if (!_schemeNames.contains(name))
        return fail(error, "方案不存在");
    return saveComplete(name, settings(name), tools, error);
}
bool SchemeCatalog::saveComplete(const QString& schemeName, const SchemeRuntimeSettings& settings,
    const QList<SchemeTool>& tools, QString* error)
{
    const auto name = schemeName.trimmed();
    if (!validName(name))
        return fail(error, "方案名称无效");
    const auto invalid = SchemeCodec::validate(settings, tools);
    if (!invalid.isEmpty())
        return fail(error, invalid);
    const auto before = *this;
    if (!_schemeNames.contains(name))
        _schemeNames.append(name);
    _settings[name] = settings;
    _tools[name] = tools;
    _currentSchemeName = name;
    if (persist(error))
        return true;
    *this = before;
    return false;
}
bool SchemeCatalog::remove(const QString& name, QString* error)
{
    if (!_schemeNames.contains(name))
        return fail(error, "方案不存在");
    const auto before = *this;
    _schemeNames.removeAll(name);
    _settings.remove(name);
    _tools.remove(name);
    if (_currentSchemeName == name)
        _currentSchemeName = _schemeNames.value(0);
    if (persist(error))
        return true;
    *this = before;
    return false;
}
bool SchemeCatalog::select(const QString& name)
{
    if (!_schemeNames.contains(name))
        return false;
    const auto before = _currentSchemeName;
    _currentSchemeName = name;
    if (persist(nullptr))
        return true;
    _currentSchemeName = before;
    return false;
}
QStringList SchemeCatalog::schemeNames() const
{
    return _schemeNames;
}
QString SchemeCatalog::currentSchemeName() const
{
    return _currentSchemeName;
}
SchemeRuntimeSettings SchemeCatalog::settings(const QString& name) const
{
    return _settings.value(name);
}
QList<SchemeTool> SchemeCatalog::tools(const QString& name) const
{
    return _tools.value(name);
}
QString SchemeCatalog::catalogPath() const
{
    return _storageDirectory + "/catalog.yaml";
}
bool SchemeCatalog::persist(QString* error) const
{
    if (!QDir().mkpath(_storageDirectory))
        return fail(error, "无法创建方案目录");
    QJsonArray schemes;
    for (const auto& name : _schemeNames) {
        auto item = SchemeCodec::encodeSettings(settings(name));
        item["name"] = name;
        item["tools"] = SchemeCodec::encodeTools(tools(name));
        schemes.append(item);
    }
    return YamlDocument::write(catalogPath(),
        { { "formatVersion", 2 }, { "currentScheme", _currentSchemeName }, { "schemes", schemes } }, error);
}
QString SchemeCatalog::portablePath(const QString& name) const
{
    QDir().mkpath(_storageDirectory);
    return _storageDirectory + "/scheme-"
        + QString::fromLatin1(
            QCryptographicHash::hash(name.toUtf8(), QCryptographicHash::Sha256).toHex().left(20))
        + ".yaml";
}
