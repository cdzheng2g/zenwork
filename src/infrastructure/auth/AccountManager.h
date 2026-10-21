#ifndef ACCOUNTMANAGER_H
#define ACCOUNTMANAGER_H

#include <QString>

// 本地账号校验。相机端为单机本地程序，登录只需与本地配置文件比对，
// 不涉及网络认证。后续如需拆分权限角色，可在accounts.ini里扩展账号分组。
class AccountManager {
public:
    static AccountManager& instance();

    bool verify(const QString& username, const QString& password) const;

    void rememberAccount(const QString& username, const QString& password);
    void forgetRememberedAccount();
    bool loadRememberedAccount(QString& username, QString& password) const;

private:
    AccountManager();

    QString accountsFilePath() const;
    QString rememberFilePath() const;
    void ensureDefaultAccount() const;

    static QString hashPassword(const QString& password, const QString& salt);
    static QByteArray obfuscate(const QByteArray& data);
};

#endif // ACCOUNTMANAGER_H
