#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include "ElaWidget.h"

class ElaLineEdit;
class ElaPushButton;
class ElaCheckBox;
class QAction;

// 相机端本地登录窗口。登录成功只发signal，不在内部决定后续跳转到哪个
// 主界面,方便后续接入相机业务主窗口时无需改动这里的逻辑。
class LoginWindow : public ElaWidget {
    Q_OBJECT
public:
    explicit LoginWindow(QWidget* parent = nullptr);
    ~LoginWindow() override;

Q_SIGNALS:
    void loginSucceeded(const QString& username);

private Q_SLOTS:
    void onLoginButtonClicked();
    void onTogglePasswordVisibility();

private:
    void initWindow();
    void initContent();
    void loadRememberedAccount();

private:
    ElaLineEdit* _usernameLineEdit { nullptr };
    ElaLineEdit* _passwordLineEdit { nullptr };
    ElaCheckBox* _rememberCheckBox { nullptr };
    ElaPushButton* _loginButton { nullptr };
    QAction* _eyeAction { nullptr };
    bool _isPasswordVisible { false };
};

#endif // LOGINWINDOW_H
