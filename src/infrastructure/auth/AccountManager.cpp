#include "infrastructure/auth/AccountManager.h"

#include <QCoreApplication>
#include <QCryptographicHash>
#include <QDir>
#include <QRandomGenerator>
#include <QSettings>
#include <QStandardPaths>

namespace {
// 编译时常量
constexpr const char* kDefaultUsername = "admin";
constexpr const char* kDefaultPassword = "admin123";
constexpr const char* kObfuscateKey = "HansAiCamera-Local-Store";
} // namespace

AccountManager& AccountManager::instance()
{
    static AccountManager manager;
    return manager;
}

AccountManager::AccountManager()
{
    // account.ini文件不存在时，创建默认账户
    ensureDefaultAccount();
}

QString AccountManager::accountsFilePath() const
{
    // 获取应用配置目录，并确保目录存在
    QString dir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QDir().mkpath(dir);
    return dir + "/accounts.ini";
}

QString AccountManager::rememberFilePath() const
{
    // 获取应用配置目录，并确保目录存在
    QString dir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QDir().mkpath(dir);
    return dir + "/remember.ini";
}

QString AccountManager::hashPassword(const QString& password, const QString& salt)
{
    // 将盐值和密码组合后进行SHA-256哈希处理
    QByteArray data = (salt + password).toUtf8();
    return QCryptographicHash::hash(data, QCryptographicHash::Sha256).toHex();
}

void AccountManager::ensureDefaultAccount() const
{
    // 检查账户文件中是否存在默认账户，如果不存在则创建
    QSettings settings(accountsFilePath(), QSettings::IniFormat);
    if (settings.contains("accounts/admin/passwordHash")) {
        return;
    }
    // 生成随机盐值，并计算默认密码的哈希值
    QString salt = QString::number(QRandomGenerator::global()->generate64(), 16);
    settings.setValue("accounts/admin/salt", salt);
    settings.setValue("accounts/admin/passwordHash", hashPassword(kDefaultPassword, salt));
    settings.setValue("accounts/admin/username", kDefaultUsername);
    // 配置持久化
    settings.sync();
}

bool AccountManager::verify(const QString& username, const QString& password) const
{
    if (username.isEmpty() || password.isEmpty()) {
        return false;
    }
    QSettings settings(accountsFilePath(), QSettings::IniFormat);
    QString storedUsername = settings.value("accounts/admin/username").toString();
    if (storedUsername.compare(username, Qt::CaseInsensitive) != 0) {
        return false;
    }
    QString salt = settings.value("accounts/admin/salt").toString();
    QString storedHash = settings.value("accounts/admin/passwordHash").toString();
    return hashPassword(password, salt) == storedHash;
}

QByteArray AccountManager::obfuscate(const QByteArray& data)
{
    QByteArray key = kObfuscateKey;
    QByteArray result;
    result.reserve(data.size());
    for (int i = 0; i < data.size(); ++i) {
        result.append(data.at(i) ^ key.at(i % key.size()));
    }
    return result;
}

void AccountManager::rememberAccount(const QString& username, const QString& password)
{
    QSettings settings(rememberFilePath(), QSettings::IniFormat);
    settings.setValue("remember/username", username);
    settings.setValue("remember/password", QString::fromLatin1(obfuscate(password.toUtf8()).toBase64()));
    settings.sync();
}

void AccountManager::forgetRememberedAccount()
{
    QSettings settings(rememberFilePath(), QSettings::IniFormat);
    settings.remove("remember");
    settings.sync();
}

bool AccountManager::loadRememberedAccount(QString& username, QString& password) const
{
    QSettings settings(rememberFilePath(), QSettings::IniFormat);
    if (!settings.contains("remember/username")) {
        return false;
    }
    username = settings.value("remember/username").toString();
    QByteArray encoded = QByteArray::fromBase64(settings.value("remember/password").toString().toLatin1());
    password = QString::fromUtf8(obfuscate(encoded));
    return true;
}
