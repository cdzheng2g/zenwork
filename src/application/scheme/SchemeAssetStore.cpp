#include "SchemeAssetStore.h"
#include "application/operators/OperatorRegistry.h"
#include <QCoreApplication>
#include <QCryptographicHash>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QRegularExpression>
#include <QSaveFile>
#include <QStandardPaths>
#include <stdexcept>
namespace {
void require(bool ok, const QString& message)
{
    if (!ok)
        throw std::runtime_error(message.toStdString());
}
QString hash(const QByteArray& b)
{
    return QString::fromLatin1(QCryptographicHash::hash(b, QCryptographicHash::Sha256).toHex());
}
QString sharedDirectory()
{
    return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/scheme_resources";
}
QString bundled(const ResourceSpec& spec)
{
    return QCoreApplication::applicationDirPath() + "/../resource/" + spec.defaultFile;
}
QString cachedPath(const QString& directory, const ResourceSpec& spec, const QString& digest)
{
    return QDir(directory).filePath(digest + "-" + QFileInfo(spec.defaultFile).fileName());
}
bool validDigest(const QString& d)
{
    return QRegularExpression("^[0-9a-f]{64}$").match(d).hasMatch();
}
QByteArray readResource(const QString& path)
{
    QFile file(path);
    require(file.open(QIODevice::ReadOnly), "无法读取模型资源：" + path);
    require(file.size() > 0 && file.size() <= 128 * 1024 * 1024, "模型资源为空或超过 128 MB");
    const auto data = file.readAll();
    require(data.size() == file.size(), "模型读取不完整");
    return data;
}
void cache(const QString& directory, const ResourceSpec& spec, const QByteArray& bytes)
{
    require(QDir().mkpath(directory), "无法创建共享资源目录");
    const auto path = cachedPath(directory, spec, hash(bytes));
    if (QFile::exists(path) && readResource(path) == bytes)
        return;
    QSaveFile file(path);
    require(file.open(QIODevice::WriteOnly) && file.write(bytes) == bytes.size() && file.commit(),
        "无法保存共享资源：" + path);
}
bool enabled(const ResourceSpec& spec, const QJsonObject& p)
{
    return spec.enabledBy.isEmpty() || p[spec.enabledBy].toBool();
}
}
QJsonObject SchemeAssetStore::encode(const QByteArray& b)
{
    return { { "sha256", hash(b) }, { "base64", QString::fromLatin1(b.toBase64()) } };
}
QByteArray SchemeAssetStore::decode(const QJsonObject& o)
{
    const auto encoded = o["base64"].toString().toLatin1(), bytes = QByteArray::fromBase64(encoded);
    require(!bytes.isEmpty() && bytes.toBase64() == encoded && hash(bytes) == o["sha256"].toString(),
        "方案资源损坏或校验失败");
    return bytes;
}
QJsonObject SchemeAssetStore::prepareForSave(SchemePackage& p, bool includeModels)
{
    QJsonObject assets;
    for (auto& tool : p.tools) {
        const auto* d = OperatorRegistry::find(tool.kind);
        require(d, "未知算子资源类型");
        for (const auto& spec : d->resources) {
            if (!enabled(spec, tool.parameters)) {
                tool.parameters[spec.key] = QString();
                continue;
            }
            const auto key = tool.id + "/" + spec.key;
            auto bytes = p.resources.value(key);
            if (bytes.isEmpty()) {
                QString source = tool.parameters[spec.key].toString();
                if (source.isEmpty() || source.startsWith("asset:")) {
                    source = cachedPath(sharedDirectory(), spec, p.resourceHashes.value(key));
                    if (!QFile::exists(source))
                        source = bundled(spec);
                }
                bytes = readResource(source);
            }
            const auto digest = hash(bytes);
            require(
                !p.resourceHashes.contains(key) || p.resourceHashes[key] == digest, "资源指纹不匹配：" + key);
            if (includeModels)
                assets[key] = encode(bytes);
            else {
                cache(sharedDirectory(), spec, bytes);
                assets[key] = QJsonObject { { "sha256", digest } };
            }
            tool.parameters[spec.key] = "asset:" + key;
        }
    }
    return assets;
}
void SchemeAssetStore::loadInto(SchemePackage& p, const QJsonObject& assets, const QString& mode, bool legacy)
{
    for (auto& tool : p.tools) {
        const auto* d = OperatorRegistry::find(tool.kind);
        require(d, "未知算子资源类型");
        for (const auto& spec : d->resources) {
            if (!enabled(spec, tool.parameters)) {
                tool.parameters[spec.key] = QString();
                continue;
            }
            const auto key = tool.id + "/" + spec.key, storedKey = legacy ? spec.key : key;
            require(tool.parameters[spec.key] == "asset:" + storedKey, "模型资源引用无效");
            const auto asset = assets[storedKey].toObject();
            const auto digest = asset["sha256"].toString();
            require(validDigest(digest), "模型指纹无效：" + key);
            p.resourceHashes[key] = digest;
            if (mode == "embedded")
                p.resources[key] = decode(asset);
            else
                require(!asset.contains("base64"), "轻量方案不应内嵌模型");
            tool.parameters[spec.key] = "asset:" + key;
        }
    }
}
bool SchemeAssetStore::install(SchemePackage& package, const QString& directory, QString* error)
{
    try {
        auto tools = package.tools;
        for (auto& tool : tools) {
            const auto* d = OperatorRegistry::find(tool.kind);
            require(d, "未知算子资源类型");
            for (const auto& spec : d->resources) {
                if (!enabled(spec, tool.parameters))
                    continue;
                const auto key = tool.id + "/" + spec.key, digest = package.resourceHashes.value(key);
                const auto bytes = package.resources.value(key);
                if (!bytes.isEmpty()) {
                    require(digest.isEmpty() || hash(bytes) == digest, "模型校验失败：" + key);
                    cache(directory, spec, bytes);
                    tool.parameters[spec.key] = cachedPath(directory, spec, hash(bytes));
                } else {
                    require(validDigest(digest), "缺少模型指纹：" + key);
                    QString found;
                    for (const auto& path : { cachedPath(directory, spec, digest), bundled(spec) })
                        if (QFile::exists(path) && hash(readResource(path)) == digest) {
                            found = path;
                            break;
                        }
                    require(!found.isEmpty(), "缺少匹配模型：" + spec.key + "（请安装模型或导入完整方案）");
                    tool.parameters[spec.key] = found;
                }
            }
        }
        package.tools = tools;
        return true;
    } catch (const std::exception& e) {
        if (error)
            *error = QString::fromUtf8(e.what());
        return false;
    }
}
