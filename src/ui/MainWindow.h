#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ElaWindow.h"

class WorkbenchWidget;

// 登录后的智能相机工作台窗口。业务导航和方案编辑布局由WorkbenchWidget负责,
// MainWindow只负责窗口生命周期和桌面环境尺寸适配。
class MainWindow : public ElaWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    void initWindow();
    void initContent();

    WorkbenchWidget* _workbench { nullptr };
};

#endif // MAINWINDOW_H
