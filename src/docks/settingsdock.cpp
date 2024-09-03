#include "settingsdock.h"

#include "duapp/app-utils.h"
#include "duapp/duui.h"
#include "duwidgets/duqfupdatedialog.h"
#include "daemon.h"
#include "enums.h"
#include "qformlayout.h"
#include "ramsettings.h"

SettingsDock::SettingsDock(QWidget *parent)
    : DuScrollArea{parent}
{
    _sm = DuSettings::i();
    setupUi();
    connectEvents();
}

void SettingsDock::updateSettings(int key, const QVariant &value)
{
    switch(key) {
    case DuSettings::UI_ToolButtonStyle:
        ui_toolButtonStyleBox->setCurrentData(value);
        break;
    case DuSettings::UI_FocusColor:
        ui_colorSelector->setColor(value.value<QColor>());
        break;
    case DuSettings::UI_IconColor:
        ui_iconColorSelector->setColor(value.value<QColor>());
        break;
    case DuSettings::UI_TrayIconMode:
        ui_trayIconModeBox->setCurrentData(value);
        break;
    case DuSettings::UI_ShowTrayIcon:
        ui_showTrayIconBox->setChecked(value.toBool());
        break;
    case DuSettings::UI_DateTimeFormat:
        ui_dateFormatBox->setCurrentData(value);
        break;
    case DuSettings::APP_CheckUpdates:
        ui_checkAtStartupBox->setChecked(value.toBool());
        break;
    case RamSettings::DaemonPort:
        ui_daemonPortBox->setValue(value.toInt());
        break;
    }
}

void SettingsDock::setupUi()
{
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setWidgetResizable(true);
    this->setMinimumWidth(350);

    QWidget *dummy = new QWidget(this);
    dummy->setProperty("class", "duTabs");
    dummy->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

    auto *mainLayout = new QVBoxLayout(dummy);
    mainLayout->setSpacing(5);
    mainLayout->setContentsMargins(6,6,6,6);

    ui_tabWidget = new DuTabWidget(dummy);
    ui_tabWidget->setTabPosition(QTabWidget::West);
    mainLayout->addWidget(ui_tabWidget);

    setupAppearanceTab();
    setupUpdatesTab();
    setupDaemonTab();

    mainLayout->addStretch();
    this->setWidget(dummy);
}

void SettingsDock::setupAppearanceTab()
{
    QWidget *w = addTab(DuIcon(":/icons/appearance90"), "Appearance");

    auto appearanceLabel = new QLabel(w);
    appearanceLabel->setText("<b>" + tr("Appearance:") + "</b>");
    w->layout()->addWidget( appearanceLabel );

    auto appearanceWidget = new QWidget(w);
    appearanceWidget->setProperty("class", "duBlock");
    w->layout()->addWidget(appearanceWidget);

    auto appearanceLayout = new QFormLayout(appearanceWidget);
    DuUI::setupLayout(appearanceLayout, 3);

    ui_toolButtonStyleBox = new DuComboBox(appearanceWidget);
    ui_toolButtonStyleBox->setShrinkable(true);
    ui_toolButtonStyleBox->addItem("Icon only", Qt::ToolButtonIconOnly);
    ui_toolButtonStyleBox->addItem("Text only", Qt::ToolButtonTextOnly);
    ui_toolButtonStyleBox->addItem("Text under icon", Qt::ToolButtonTextUnderIcon);
    ui_toolButtonStyleBox->addItem("Text beside icon", Qt::ToolButtonTextBesideIcon);
    ui_toolButtonStyleBox->setCurrentData( _sm->get(DuSettings::UI_ToolButtonStyle).toInt() );

    appearanceLayout->addRow(tr("Tool buttons style"), ui_toolButtonStyleBox);

    ui_colorSelector = new DuColorSelector(appearanceWidget);
    ui_colorSelector->setColor( _sm->get(DuSettings::UI_FocusColor).value<QColor>() );

    appearanceLayout->addRow(tr("Focus color"), ui_colorSelector);

    ui_iconColorSelector = new DuColorSelector(appearanceWidget);
    ui_iconColorSelector->setColor( _sm->get(DuSettings::UI_IconColor).value<QColor>() );

    appearanceLayout->addRow(tr("Icons color"), ui_iconColorSelector);

    ui_showTrayIconBox = new QCheckBox(tr("Always show"), this);
    ui_showTrayIconBox->setChecked( _sm->get(DuSettings::UI_ShowTrayIcon).toBool() );

    appearanceLayout->addRow(tr("Tray icon"), ui_showTrayIconBox);

    ui_trayIconModeBox = new DuComboBox(this);
    ui_trayIconModeBox->addItem(tr("Dark"), DarkerColor);
    ui_trayIconModeBox->addItem(tr("Color"), NormalColor);
    ui_trayIconModeBox->addItem(tr("Light"), LighterColor);
    ui_trayIconModeBox->setCurrentData( _sm->get(DuSettings::UI_TrayIconMode) );

    appearanceLayout->addRow(tr("Tray icon color"), ui_trayIconModeBox);

    ui_dateFormatBox = new DuComboBox(this);
    QDateTime d = QDateTime::fromString("2021-04-26 10:53:31", "yyyy-MM-dd hh:mm:ss");
    QString f = "yyyy-MM-dd hh:mm:ss";
    ui_dateFormatBox->addItem(d.toString(f), f);
    f = "MM.dd.yyyy - h:mm:ss ap";
    ui_dateFormatBox->addItem(d.toString(f), f);
    f = "dd/MM/yyyy - hh:mm:ss";
    ui_dateFormatBox->addItem(d.toString(f), f);
    f = "ddd MMMM d yyyy 'at' h:mm:ss ap";
    ui_dateFormatBox->addItem(d.toString(f), f);
    f = "ddd d MMMM yyyy 'at' hh:mm:ss";
    ui_dateFormatBox->addItem(d.toString(f), f);
    ui_dateFormatBox->setCurrentData( _sm->get(DuSettings::UI_DateTimeFormat) );

    appearanceLayout->addRow(tr("Date format"), ui_dateFormatBox);

    auto l = qobject_cast<QVBoxLayout*>( w->layout() );
    l->addStretch();
}

void SettingsDock::setupUpdatesTab()
{
    QWidget *w = addTab(DuIcon(":/icons/update90"), "Updates");

    auto updatesLabel = new QLabel("<b>" + tr("Updates:") + "</b>", w);
    w->layout()->addWidget( updatesLabel );

    auto updatesWidget = new QWidget(w);
    updatesWidget->setProperty("class", "duBlock");
    w->layout()->addWidget(updatesWidget);

    auto updatesLayout = new QVBoxLayout(updatesWidget);
    DuUI::setupLayout(updatesLayout, 3);

    ui_checkAtStartupBox = new QCheckBox("Check during startup", this);
    ui_checkAtStartupBox->setChecked(_sm->get(DuSettings::APP_CheckUpdates).toBool());
    updatesLayout->addWidget(ui_checkAtStartupBox);

    ui_checkNowButton = new QPushButton("Check now", this);
    ui_checkNowButton->setIcon(DuIcon(":/icons/check-update"));
    updatesLayout->addWidget(ui_checkNowButton);

    auto l = qobject_cast<QVBoxLayout*>( w->layout() );
    l->addStretch();
}

void SettingsDock::setupDaemonTab()
{
    QWidget *w = addTab(DuIcon(":/icons/daemon90"), "Daemon");

    auto daemonLabel = new QLabel("<b>" + tr("Ramses daemon:") + "</b>", w);
    w->layout()->addWidget( daemonLabel );

    auto daemonWidget = new QWidget(w);
    daemonWidget->setProperty("class", "duBlock");
    w->layout()->addWidget(daemonWidget);

    auto daemonLayout = new QVBoxLayout(daemonWidget);
    DuUI::setupLayout(daemonLayout, 3);

    ui_daemonPortBox = new DuSpinBox(w);
    ui_daemonPortBox->setPrefix("Port: ");
    ui_daemonPortBox->setMinimum(1024);
    ui_daemonPortBox->setMaximum(65535);
    ui_daemonPortBox->setValue(18185);
    daemonLayout->addWidget(ui_daemonPortBox);

    ui_restartDaemonButton = new QPushButton("Restart Daemon");
    daemonLayout->addWidget(ui_restartDaemonButton);

    auto l = qobject_cast<QVBoxLayout*>( w->layout() );
    l->addStretch();
}

void SettingsDock::connectEvents()
{
    connect(_sm, &DuSettings::settingChanged,
            this, &SettingsDock::updateSettings);

    connect( ui_toolButtonStyleBox, QOverload<int>::of(&DuComboBox::activated),
            this, [this] (int i) {
                _sm->set( DuSettings::UI_ToolButtonStyle,
                         ui_toolButtonStyleBox->itemData(i).toInt()
                         );
            } );

    connect( ui_colorSelector, &DuColorSelector::colorChanged,
            this, [this] (const QColor &color) {
                _sm->set(DuSettings::UI_FocusColor, color);
            });

    connect( ui_iconColorSelector, &DuColorSelector::colorChanged,
            this, [this] (const QColor &color) {
                _sm->set(DuSettings::UI_IconColor, color);
            });

    connect( ui_trayIconModeBox, &DuComboBox::dataActivated,
            this, [this] (const QVariant &val) {
                _sm->set(DuSettings::UI_TrayIconMode, val);
            });

    connect( ui_showTrayIconBox, &QCheckBox::clicked,
            this, [this] (bool checked) {
                _sm->set(DuSettings::UI_ShowTrayIcon, checked);
            });

    connect( ui_dateFormatBox, &DuComboBox::dataActivated,
            this, [this] (const QVariant &val) {
                _sm->set(DuSettings::UI_DateTimeFormat, val);
            });

    connect( ui_checkAtStartupBox, &QCheckBox::clicked,
            this, [this] (bool checked) {
                _sm->set(DuSettings::APP_CheckUpdates, checked);
            });

    connect( ui_checkNowButton, &QPushButton::clicked, this, [] () {
        DuApplication *app = qobject_cast<DuApplication*>(qApp);
        app->checkUpdate();
        DuQFUpdateDialog dialog( app->updateInfo() );
        dialog.exec();
    });

    connect( ui_daemonPortBox, &DuSpinBox::editingFinished,
            this, [this] () {
                _sm->set(RamSettings::DaemonPort, ui_daemonPortBox->value());
            });

    connect( ui_restartDaemonButton, &QPushButton::clicked, Daemon::instance(), &Daemon::restart );
}

QWidget *SettingsDock::addTab(const DuIcon &icon, const QString &name)
{
    auto *w = new QWidget(ui_tabWidget);
    ui_tabWidget->addTab(w, icon, "");
    ui_tabWidget->setTabToolTip(ui_tabWidget->count()-1, name);

    auto l = new QVBoxLayout(w);
    DuUI::setupLayout(l, 3);

    return w;
}
