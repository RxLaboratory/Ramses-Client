#ifndef SETTINGSDOCK_H
#define SETTINGSDOCK_H

#include "duapp/dusettings.h"
#include "duwidgets/ducombobox.h"
#include "duwidgets/duicon.h"
#include "duwidgets/duscrollarea.h"
#include "duwidgets/dutabwidget.h"
#include "duwidgets/ducolorselector.h"
#include "duwidgets/duspinbox.h"
#include "qcheckbox.h"
#include "qpushbutton.h"

class SettingsDock : public DuScrollArea
{
    Q_OBJECT
public:
    SettingsDock(QWidget *parent = nullptr);
private slots:
    void updateSettings(int key, const QVariant &value);
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
    DuSettings *_sm;

    // Utils
    QWidget *addTab(const DuIcon &icon, const QString &name);
};

#endif // SETTINGSDOCK_H
