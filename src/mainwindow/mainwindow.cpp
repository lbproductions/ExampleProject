#include "mainwindow.h"

#include "actions.h"
#include "controller.h"
#include "sidebar.h"
#include "views/sampleview.h"

#include <QCloseEvent>
#include <QStackedWidget>

MainWindow::MainWindow() :
    LBGui::MainWindow(),
    m_controller(new MainWindowNS::Controller(this)),
    m_sidebar(new MainWindowNS::Sidebar(m_controller))
{
    setSettingsName(QLatin1String("MainWindow"));
    setTitle(tr("Main Window"));

    setupSidebar();
    setupCentralWidgets();
    setupMenuBar();
    setupToolBar();

    //save and restore window size and position
    setSaveSettings(true);
    restoreSettings();
}

void MainWindow::setupCentralWidgets()
{
    // add possible central widgets:
    //centralViewStackedWidget()->addWidget(new QWidget(centralViewStackedWidget()));

    // add a grey background as starting widget
    LBGui::BackgroundWidget *background = new LBGui::BackgroundWidget(centralViewStackedWidget());
    background->setStyle(LBGui::BackgroundWidget::Gray);
    centralViewStackedWidget()->addWidget(background);
    centralViewStackedWidget()->setCurrentWidget(background);
}

void MainWindow::setupSidebar()
{
    //add categories to the sidebar

    m_sidebar->addParentCategorie(tr("Library"));
    m_sidebar->addChildCategorie(0,tr("Example 1"))->setWidget(new MainWindowNS::SampleView(tr("Example 1")));
    m_sidebar->addChildCategorie(0,tr("Example 2"))->setWidget(new MainWindowNS::SampleView(tr("Example 2")));

    m_sidebar->expandAll();

    setLeftSideBar(m_sidebar);
}

void MainWindow::setupMenuBar()
{
    //add actions to the menu bar
    menuBar()->addAction(tr("&File"), m_controller->actions()->exampleAction());
    menuBar()->addAction(tr("&File/Sub&menu"), m_controller->actions()->exampleAction());
    menuBar()->addAction(tr("&File"), m_controller->actions()->checkForUpdatesAction());
    menuBar()->addAction(tr("&File"), m_controller->actions()->showPreferencesAction());

    menuBar()->addMenu(menuBar()->windowMenu());
    menuBar()->addMenu(menuBar()->helpMenu());
    menuBar()->addWindow(this);
}

void MainWindow::setupToolBar()
{
    LBGui::ToolBar *toolBar = new LBGui::ToolBar(this);

    //add actions to the tool bar
    toolBar->addAction(m_controller->actions()->exampleAction());

    setUnifiedTitleAndToolBarOnMac(true);
    addToolBar(toolBar);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(m_controller->close()) {
        saveSettings();
        event->accept();
    }
    else {
        event->ignore();
    }
}
