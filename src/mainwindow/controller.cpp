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
#include <QSettings>

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

    QSettings settings;
    bool checksForUpdates = settings.value(LBGui::UpdaterPreferences::AutomaticChecksSetting, QVariant(true)).toBool();

    if (m_updater) {
        m_updater->setAutomaticallyChecksForUpdates(checksForUpdates);
        if(checksForUpdates)
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

void Controller::showPreferences()
{
    LBGui::PreferencesWindow *window = new LBGui::PreferencesWindow;
    LBGui::UpdaterPreferences *updaterPreferences = new LBGui::UpdaterPreferences(window);
    connect(updaterPreferences, SIGNAL(automaticChecksToggled(bool)), this, SLOT(toggleAutomaticUpdates(bool)));
    connect(updaterPreferences, SIGNAL(checkForUpdatesRequested()), this, SLOT(checkForUpdates()));

    window->addPage(updaterPreferences);

    window->show();
}

void Controller::showWidget(QWidget *widget)
{
    m_mainWindow->setCentralWidget(widget);
    emit somethingChanged();
}

void Controller::toggleAutomaticUpdates(bool check)
{
    if (m_updater)
        m_updater->setAutomaticallyChecksForUpdates(check);
}

} // namespace MainWindowNS
