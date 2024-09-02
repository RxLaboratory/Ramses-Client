#ifndef SETTINGSDOCK_H
#define SETTINGSDOCK_H

#include "duqf-app/dusettingsmanager.h"
#include "duqf-widgets/ducombobox.h"
#include "duqf-widgets/duicon.h"
#include "duqf-widgets/duscrollarea.h"
#include "duqf-widgets/dutabwidget.h"
#include "duqf-widgets/ducolorselector.h"
#include "duqf-widgets/duspinbox.h"
#include "qcheckbox.h"
#include "qpushbutton.h"

class SettingsDock : public DuScrollArea
{
    Q_OBJECT
public:
    SettingsDock(QWidget *parent = nullptr);
private:
    void setupUi();
    void setupAppearanceTab();
    void setupUpdatesTab();
    void setupDaemonTab();
    void connectEvents();

    // Widgets
    DuTabWidget *ui_tabWidget;

    DuComboBox *ui_toolButtonStyleBox;
    DuColorSelector *ui_colorSelector;
    DuColorSelector *ui_iconColorSelector;
    DuComboBox *ui_trayIconModeBox;
    QCheckBox *ui_showTrayIconBox;
    DuComboBox *ui_dateFormatBox;

    QCheckBox *ui_checkAtStartupBox;
    QPushButton *ui_checkNowButton;

    DuSpinBox *ui_daemonPortBox;
    QPushButton *ui_restartDaemonButton;

    // Shortcut
    DuSettingsManager *_sm;

    // Utils
    QWidget *addTab(const DuIcon &icon, const QString &name);
};

#endif // SETTINGSDOCK_H
