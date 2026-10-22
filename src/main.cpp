#include "ElaApplication.h"
#include "LoginWindow.h"
#include "MainWindow.h"
#include "contracts/ReferenceImage.h"
#include "infrastructure/camera/CameraController.h"
#include "infrastructure/camera/CameraStreamService.h"
#include "infrastructure/inference/InspectionProcess.h"
#include <cstring>

#include <opencv2/core.hpp>

#include <QApplication>
#include <QMetaType>

int main(int argc, char* argv[])
{
    if (argc > 1 && std::strcmp(argv[1], "--inspection-worker") == 0)
        return inspectionWorkerMain(argc, argv);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#endif
    // Camera frames cross the queued connection from the capture thread.
    qRegisterMetaType<cv::Mat>("cv::Mat");
    qRegisterMetaType<FrameEnvelope>("FrameEnvelope");
    // 无桌面环境(eglfs/linuxfb)下没有系统级输入法/虚拟键盘,
    // 接入第三方 QWidget 虚拟键盘插件(third_party/QVirtualKeyboard)
    qputenv("QT_IM_MODULE", QByteArray("Qt5Input"));
    QApplication app(argc, argv);
    // xcb/eglfs 等平台默认关闭自动弹出输入面板(QApplication::autoSipEnabled
    // 默认仅在触屏优先的平台上为true),需要显式开启才能在点击输入框时弹出软键盘
    app.setAutoSipEnabled(true);
    eApp->init();

    // 板端 VI->VPSS 取帧前启动 AIQ，使传感器曝光和白平衡正常工作。
    if (!CameraController::instance().initialize())
        qWarning("相机ISP初始化失败：将继续启动界面，但画面可能未完成3A处理");
    QObject::connect(&app, &QCoreApplication::aboutToQuit, [] {
        CameraStreamService::instance().shutdown();
        CameraController::instance().shutdown();
    });

    LoginWindow loginWindow;
    static MainWindow* mainWindow = nullptr;
    QObject::connect(
        &loginWindow, &LoginWindow::loginSucceeded, &loginWindow, [&loginWindow](const QString& username) {
            Q_UNUSED(username)
            mainWindow = new MainWindow();
            mainWindow->show();
            loginWindow.close();
        });
    loginWindow.show();

    return app.exec();
}
