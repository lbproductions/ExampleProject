#ifndef LBGUI_UPDATERPREFERENCES_H
#define LBGUI_UPDATERPREFERENCES_H

#include "preferencespage.h"

namespace LBGui {

class UpdaterPreferences : public PreferencesPage
{
    Q_OBJECT
public:
    explicit UpdaterPreferences(PreferencesWindow *parent);

    static const QString AutomaticChecksSetting;

signals:
    void checkForUpdatesRequested();
    void automaticChecksToggled(bool);

private Q_SLOTS:
    void toggleAutomaticChecks();
    
};

} // namespace LBGui

#endif // LBGUI_UPDATERPREFERENCES_H
