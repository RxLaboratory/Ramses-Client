#ifndef SETTINGSDOCKWIDGET_H
#define SETTINGSDOCKWIDGET_H

#include <QBoxLayout>
#include <QPushButton>
#include <QCheckBox>

#include "duapp/dusettings.h"
#include "duwidgets/ducombobox.h"
#include "duwidgets/duicon.h"
#include "duwidgets/duscrollarea.h"
#include "duwidgets/dutabwidget.h"
#include "duwidgets/ducolorselector.h"
#include "duwidgets/duspinbox.h"

class SettingsDockWidget : public DuScrollArea
{
    Q_OBJECT
public:
    SettingsDockWidget(QWidget *parent = nullptr);


private slots:
    void updateSettings(int key, const QVariant &value);
    void loadThemes();
    void setTheme(int t);

private:
    void setupUi();

    void setupAppearanceTab();
    void setupUpdatesTab();
    void setupDaemonTab();
    void connectEvents();

    // Widgets
    DuTabWidget *ui_tabWidget;

    DuComboBox *ui_toolButtonStyleBox;
    DuSpinBox *ui_marginsBox;
    DuComboBox *ui_themeBox;
    QWidget *ui_colorsWidget;
    DuColorSelector *ui_focusColorSelector;
    DuColorSelector *ui_iconColorSelector;
    DuColorSelector *ui_fgColorSelector;
    DuColorSelector *ui_bgColorSelector;
    DuComboBox *ui_contrastBox;
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
    QWidget *addSection(const QString &name, QBoxLayout *layout);
    QJsonObject getTheme(const QString &path) const;
};

#endif // SETTINGSDOCKWIDGET_H
