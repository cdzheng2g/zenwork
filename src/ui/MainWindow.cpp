#include "MainWindow.h"

#include "workbench/WorkbenchWidget.h"

#include <QGuiApplication>
#include <QScreen>

MainWindow::MainWindow(QWidget* parent)
    : ElaWindow(parent)
{
    initWindow();
    initContent();
}

void MainWindow::initWindow()
{
    setWindowTitle(QStringLiteral("HANS.AI 智能相机"));

    const QRect availableRect = QGuiApplication::primaryScreen()->availableGeometry();

    setMinimumSize(QSize(800, 480));
    setGeometry(availableRect);

    setIsNavigationBarEnable(false);
    setUserInfoCardVisible(false);
}

void MainWindow::initContent()
{
    _workbench = new WorkbenchWidget(this);
    setCentralCustomWidget(_workbench);
}
