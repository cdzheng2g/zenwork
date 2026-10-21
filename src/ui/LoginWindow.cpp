#include "LoginWindow.h"

#include "infrastructure/auth/AccountManager.h"
#include "ElaCheckBox.h"
#include "ElaIcon.h"
#include "ElaLineEdit.h"
#include "ElaMessageBar.h"
#include "ElaPushButton.h"
#include "ElaText.h"

#include <QAction>
#include <QGridLayout>
#include <QGuiApplication>
#include <QScreen>
#include <QVBoxLayout>

LoginWindow::LoginWindow(QWidget* parent)
    : ElaWidget(parent)
{
    initWindow();
    initContent();
    loadRememberedAccount();
}

LoginWindow::~LoginWindow() { }

void LoginWindow::initWindow()
{
    // 3576板子上是eglfs/linuxfb裸屏,没有窗口管理器,
    // 窗口铺满整块屏幕,不保留AppBar(最小化/关闭按钮在无桌面环境下没有实际作用)
    setWindowTitle("工业智能相机 - 登录");
    setWindowButtonFlags(ElaAppBarType::NoneButtonHint);
    setAppBarHeight(0);
    resize(QGuiApplication::primaryScreen()->size());
}

void LoginWindow::initContent()
{
    setStyleSheet(QStringLiteral(
        "LoginWindow { background:#f5f3f0; }"
        "QWidget#loginCard { background:#ffffff; border:1px solid #d8dcdf; border-top:5px solid #a91427; }"
        "ElaLineEdit { background:#ffffff; border:1px solid #bfc7ce; border-radius:3px; padding:0 10px; "
        "color:#202833; }"
        "ElaLineEdit:focus { border:2px solid #3b9ced; }"
        "ElaPushButton { background:#a91427; color:#ffffff; border:0; border-radius:3px; font-weight:600; }"
        "ElaPushButton:hover { background:#7e0c1b; }"));
    // 登录卡片保持固定舒适尺寸,居中显示在铺满屏幕的背景上,
    // 而不是让表单跟着整块屏幕拉伸变形
    const QRect screenGeometry = QGuiApplication::primaryScreen()->availableGeometry();
    const int cardWidth = qBound(360, static_cast<int>(screenGeometry.width() * 0.35), 480);
    const int cardHeight = qBound(460, static_cast<int>(screenGeometry.height() * 0.65), 620);

    QWidget* centralWidget = new QWidget(this);
    centralWidget->setObjectName(QStringLiteral("loginCard"));
    centralWidget->setFixedSize(cardWidth, cardHeight);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(48, 36, 48, 36);
    mainLayout->setSpacing(18);

    QGridLayout* rootLayout = new QGridLayout(this);
    rootLayout->setContentsMargins(0, getAppBarHeight(), 0, 0);
    rootLayout->addWidget(centralWidget, 0, 0, Qt::AlignCenter);

    ElaText* logoText = new ElaText("HANS.AI 工业智能相机", centralWidget);
    logoText->setTextPixelSize(22);
    logoText->setAlignment(Qt::AlignCenter);

    ElaText* subTitleText = new ElaText("请输入账号密码登录", centralWidget);
    subTitleText->setTextPixelSize(13);
    subTitleText->setAlignment(Qt::AlignCenter);

    _usernameLineEdit = new ElaLineEdit(centralWidget);
    _usernameLineEdit->setFixedHeight(38);
    _usernameLineEdit->setPlaceholderText("用户名");

    _passwordLineEdit = new ElaLineEdit(centralWidget);
    _passwordLineEdit->setFixedHeight(38);
    _passwordLineEdit->setPlaceholderText("密码");
    _passwordLineEdit->setEchoMode(QLineEdit::Password);
    _passwordLineEdit->setIsClearButtonEnable(false);

    _eyeAction = new QAction(_passwordLineEdit);
    _eyeAction->setIcon(ElaIcon::getInstance()->getElaIcon(ElaIconType::Eye, 14));
    _passwordLineEdit->addAction(_eyeAction, QLineEdit::TrailingPosition);
    connect(_eyeAction, &QAction::triggered, this, &LoginWindow::onTogglePasswordVisibility);

    _rememberCheckBox = new ElaCheckBox("记住密码", centralWidget);

    _loginButton = new ElaPushButton("登录", centralWidget);
    _loginButton->setFixedHeight(38);
    connect(_loginButton, &ElaPushButton::clicked, this, &LoginWindow::onLoginButtonClicked);
    connect(_passwordLineEdit, &ElaLineEdit::returnPressed, this, &LoginWindow::onLoginButtonClicked);

    mainLayout->addStretch(1);
    mainLayout->addWidget(logoText);
    mainLayout->addWidget(subTitleText);
    mainLayout->addSpacing(12);
    mainLayout->addWidget(_usernameLineEdit);
    mainLayout->addWidget(_passwordLineEdit);
    mainLayout->addWidget(_rememberCheckBox);
    mainLayout->addSpacing(6);
    mainLayout->addWidget(_loginButton);
    mainLayout->addStretch(2);
}

void LoginWindow::loadRememberedAccount()
{
    QString username;
    QString password;
    if (AccountManager::instance().loadRememberedAccount(username, password)) {
        _usernameLineEdit->setText(username);
        _passwordLineEdit->setText(password);
        _rememberCheckBox->setChecked(true);
    }
}

void LoginWindow::onTogglePasswordVisibility()
{
    _isPasswordVisible = !_isPasswordVisible;
    _passwordLineEdit->setEchoMode(_isPasswordVisible ? QLineEdit::Normal : QLineEdit::Password);
    _eyeAction->setIcon(ElaIcon::getInstance()->getElaIcon(
        _isPasswordVisible ? ElaIconType::EyeSlash : ElaIconType::Eye, 14));
}

void LoginWindow::onLoginButtonClicked()
{
    const QString username = _usernameLineEdit->text().trimmed();
    const QString password = _passwordLineEdit->text();

    if (username.isEmpty() || password.isEmpty()) {
        ElaMessageBar::warning(ElaMessageBarType::Top, "提示", "用户名和密码不能为空", 2000, this);
        return;
    }

    if (!AccountManager::instance().verify(username, password)) {
        ElaMessageBar::error(ElaMessageBarType::Top, "登录失败", "用户名或密码错误", 2000, this);
        return;
    }

    if (_rememberCheckBox->isChecked()) {
        AccountManager::instance().rememberAccount(username, password);
    } else {
        AccountManager::instance().forgetRememberedAccount();
    }

    Q_EMIT loginSucceeded(username);
}
