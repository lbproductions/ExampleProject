#include "actions.h"

#include "controller.h"

#include <LBGui/LBGui.h>


namespace MainWindowNS {

Actions::Actions(Controller *controller) :
    QObject(controller),
    m_controller(controller)
{
    // connect changes (like open files, or selected text etc) to the updateActions slot
    //connect(controller, SIGNAL(somethingChanged()), this, SLOT(updateActions()));

    m_exampleAction = new LBGui::Action(this);
    m_exampleAction->setText(tr("&Example..."));
    m_exampleAction->setShortcut(QKeySequence::Open);
    connect(m_exampleAction, SIGNAL(triggered()), m_controller, SLOT(example()));


    m_checkForUpdatesAction = new LBGui::Action(this);
    m_checkForUpdatesAction->setText(tr("&Check for updates..."));
    m_checkForUpdatesAction->setMenuRole(QAction::ApplicationSpecificRole);
    connect(m_checkForUpdatesAction, SIGNAL(triggered()), m_controller, SLOT(checkForUpdates()));


    m_showPreferencesAction = new LBGui::Action(this);
    m_showPreferencesAction->setText(tr("&Preferences..."));
    m_showPreferencesAction->setShortcut(QKeySequence::Preferences);
    connect(m_showPreferencesAction, SIGNAL(triggered()), m_controller, SLOT(showPreferences()));
}

LBGui::Action *Actions::exampleAction() const
{
    return m_exampleAction;
}

LBGui::Action *Actions::checkForUpdatesAction() const
{
    return m_checkForUpdatesAction;
}

LBGui::Action *Actions::showPreferencesAction() const
{
    return m_showPreferencesAction;
}

void Actions::updateActions()
{
    // enable actions, based on some conditions (like open files, or selected text etc)
    m_exampleAction->setEnabled(true);
}

} // namespace MainWindowNS
