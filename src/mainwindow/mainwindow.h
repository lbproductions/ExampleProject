#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <LBGui/LBGui.h>

namespace MainWindowNS {
class Controller;
class Sidebar;
}

class MainWindow : public LBGui::MainWindow
{
public:
    MainWindow();

    void closeEvent(QCloseEvent *event);

private:
    void setupCentralWidgets();
    void setupSidebar();
    void setupMenuBar();
    void setupToolBar();

    MainWindowNS::Controller *m_controller;
    MainWindowNS::Sidebar *m_sidebar;
};

#endif // MAINWINDOW_H
