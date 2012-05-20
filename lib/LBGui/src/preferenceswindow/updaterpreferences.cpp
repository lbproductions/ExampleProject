#include "updaterpreferences.h"

#include <QCheckBox>
#include <QFormLayout>
#include <QPushButton>
#include <QSettings>

namespace LBGui {

const QString UpdaterPreferences::AutomaticChecksSetting("updater/checkAutomatically");

UpdaterPreferences::UpdaterPreferences(PreferencesWindow *parent) :
    PreferencesPage(parent)
{
    setName(tr("Updates"));
    setIcon(QIcon(":/preferenceswindow/mac/apple-update"));

    QSettings settings;
    QFormLayout *layout = new QFormLayout(this);

    QCheckBox *checkAutomaticallyCheckBox = new QCheckBox(this);
    bool checkAutomatically = settings.value(AutomaticChecksSetting, QVariant(true)).toBool();
    checkAutomaticallyCheckBox->setText(tr("Check for updates automatically"));
    checkAutomaticallyCheckBox->setChecked(checkAutomatically);
    connect(checkAutomaticallyCheckBox, SIGNAL(clicked()), this, SLOT(toggleAutomaticChecks()));
    connect(checkAutomaticallyCheckBox, SIGNAL(toggled(bool)), this, SIGNAL(automaticChecksToggled(bool)));
    layout->addRow(tr("Updates:"), checkAutomaticallyCheckBox);

    QPushButton *checkNowButton = new QPushButton(this);
    checkNowButton->setText(tr("Check now"));
    connect(checkNowButton, SIGNAL(clicked()), this, SIGNAL(checkForUpdatesRequested()));
    layout->addRow(tr(""), checkNowButton);

    setLayout(layout);
}

void UpdaterPreferences::toggleAutomaticChecks()
{
    QSettings settings;
    bool checkAutomatically = settings.value(AutomaticChecksSetting, QVariant(true)).toBool();

    settings.setValue(AutomaticChecksSetting, QVariant(!checkAutomatically));
}

} // namespace LBGui
