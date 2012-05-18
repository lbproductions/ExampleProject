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
    connect(m_exampleAction, SIGNAL(triggered()), m_controller, SLOT(exampleAction()));
}

LBGui::Action *Actions::exampleAction() const
{
    return m_exampleAction;
}

void Actions::updateActions()
{
    // enable actions, based on some conditions (like open files, or selected text etc)
    m_exampleAction->setEnabled(true);
}

} // namespace MainWindowNS
