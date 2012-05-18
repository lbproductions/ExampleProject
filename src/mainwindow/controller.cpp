#include "controller.h"

#include "mainwindow.h"

#include <LBGui/LBGui.h>

#include "actions.h"

#include <QApplication>
#include <QMessageBox>


namespace MainWindowNS {

Controller::Controller(MainWindow *mainWindow) :
    QObject(mainWindow),
    m_mainWindow(mainWindow),
    m_actions(new Actions(this))
{
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

void Controller::exampleAction()
{
    QMessageBox msg;
    msg.setText("Example");
    msg.exec();

    emit somethingChanged();
}

void Controller::showWidget(QWidget *widget)
{
    m_mainWindow->setCentralWidget(widget);
    emit somethingChanged();
}

} // namespace MainWindowNS
