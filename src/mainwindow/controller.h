#ifndef MAINWINDOW_CONTROLLER_H
#define MAINWINDOW_CONTROLLER_H

#include <QObject>

class MainWindow;
class Updater;

namespace MainWindowNS {

class Actions;

class Controller : public QObject
{
    Q_OBJECT
public:
    explicit Controller(MainWindow *mainWindow);

    MainWindow *mainWindow() const;
    Actions *actions();

    bool close();

public slots:
    void example();
    void checkForUpdates();
    void showPreferences();

    void showWidget(QWidget *widget);

signals:
    void somethingChanged();

private slots:
    void toggleAutomaticUpdates(bool);

private:
    MainWindow *m_mainWindow;
    Actions *m_actions;
    Updater *m_updater;
};

} // namespace MainWindowNS

#endif // MAINWINDOW_CONTROLLER_H
