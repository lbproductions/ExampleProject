#include "controller.h"

#include "actions.h"
#include "mainwindow.h"

#ifdef Q_WS_MAC
#   include "../misc/sparkleupdater.h"
#elif defined Q_WS_WIN
#   include "../misc/winsparkleupdater.h"
#endif

#include <LBGui/LBGui.h>

#include <QApplication>
#include <QMessageBox>
#include <QDebug>

namespace MainWindowNS {

Controller::Controller(MainWindow *mainWindow) :
    QObject(mainWindow),
    m_mainWindow(mainWindow),
    m_actions(new Actions(this))
{
#ifdef Q_WS_MAC
    m_updater = new SparkleUpdater(APPCASTURL"/AppCast_mac.xml");
#elif defined Q_WS_WIN
    m_updater = new WinSparkleUpdater("APPCASTURL/AppCast_win.xml");
#endif

    if (m_updater) {
        m_updater->checkForUpdatesInBackground();
    }
}

MainWindow *Controller::mainWindow() const
{
    return m_mainWindow;
}

Actions *Controller::actions()
{
    return m_actions;
}

bool Controller::close()
{
    //somehow confirm the closing of the main window
    return true;
}

void Controller::example()
{
    QMessageBox msg;
    msg.setText("Example");
    msg.exec();

    emit somethingChanged();
}

void Controller::checkForUpdates()
{
    if(m_updater)
        m_updater->checkForUpdates();
}

void Controller::showWidget(QWidget *widget)
{
    m_mainWindow->setCentralWidget(widget);
    emit somethingChanged();
}

} // namespace MainWindowNS
