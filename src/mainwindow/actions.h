#ifndef MAINWINDOW_ACTIONS_H
#define MAINWINDOW_ACTIONS_H

#include <QObject>

namespace LBGui {
class Action;
}

namespace MainWindowNS {

class Controller;

class Actions : public QObject
{
    Q_OBJECT
public:
    explicit Actions(Controller *controller);

    LBGui::Action *exampleAction() const;

public slots:
    void updateActions();

private:
    Controller *m_controller;

    LBGui::Action *m_exampleAction;

};

} // namespace MainWindowNS

#endif // MAINWINDOW_ACTIONS_H