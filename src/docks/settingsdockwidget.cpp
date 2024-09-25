#include "settingsdockwidget.h"

#include <QDir>

#include "duapp/app-config.h"
#include "duapp/app-utils.h"
#include "duapp/duui.h"
#include "duwidgets/duqfupdatedialog.h"
#include "daemon.h"
#include "ramsettings.h"

SettingsDockWidget::SettingsDockWidget(QWidget *parent)
    : DuScrollArea{parent}
{
    _sm = DuSettings::i();
    setupUi();
    connectEvents();
}

void SettingsDockWidget::updateSettings(int key, const QVariant &value)
{
    switch(key) {
    case DuSettings::UI_ToolButtonStyle:
        ui_toolButtonStyleBox->setCurrentData(value);
        break;
    case DuSettings::UI_IconColor:
        ui_iconColorSelector->setColor(value.value<QColor>());
        break;
    case DuSettings::UI_FocusColor:
        ui_focusColorSelector->setColor(value.value<QColor>());
        break;
    case DuSettings::UI_ForegroundColor:
        ui_fgColorSelector->setColor(value.value<QColor>());
        break;
    case DuSettings::UI_BackgroundColor:
        ui_bgColorSelector->setColor(value.value<QColor>());
        break;
    case DuSettings::UI_Contrast:
        ui_contrastBox->setCurrentData(value.toString());
        break;
    case DuSettings::UI_TrayIconMode:
        ui_trayIconModeBox->setCurrentData(value);
        break;
    case DuSettings::UI_ShowTrayIcon:
        ui_showTrayIconBox->setChecked(value.toBool());
        break;
    case DuSettings::UI_Margins:
        ui_marginsBox->setValue(value.toInt());
        break;
    case DuSettings::UI_TimeFormat:
        ui_timeFormatBox->setCurrentData(value);
        break;
    case DuSettings::UI_DateFormat:
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

void SettingsDockWidget::setupUi()
{
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setWidgetResizable(true);
    this->setMinimumWidth(350);

    QWidget *dummy = new QWidget(this);
    dummy->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

    auto *mainLayout = DuUI::addBoxLayout(Qt::Vertical, dummy);
    int m = DuSettings::i()->get(DuSettings::UI_Margins).toInt();
    mainLayout->setSpacing(m*2);
    mainLayout->setContentsMargins(m*3,m*3,m*3,m*3);

    ui_tabWidget = new DuTabWidget(dummy);
    ui_tabWidget->setTabPosition(QTabWidget::West);
    mainLayout->addWidget(ui_tabWidget);

    setupAppearanceTab();
    setupUpdatesTab();
    setupDaemonTab();

    mainLayout->addStretch();
    this->setWidget(dummy);
}

void SettingsDockWidget::setupAppearanceTab()
{
    QWidget *w = addTab(DuIcon(":/icons/appearance90"), "Appearance");
    auto l = qobject_cast<QBoxLayout*>( w->layout() );

    QWidget *themeWidget = addSection(tr("Theme"), l);
    auto themeLayout = qobject_cast<QFormLayout*>( themeWidget->layout() );

    ui_themeBox = new DuComboBox(this);
    ui_themeBox->setShrinkable(true);
    loadThemes();
    themeLayout->addRow(tr("Preset"), ui_themeBox);

    QWidget *appearanceWidget = addSection(tr("Appearance"), l);
    auto appearanceLayout = qobject_cast<QFormLayout*>( appearanceWidget->layout() );

    ui_toolButtonStyleBox = new DuComboBox(appearanceWidget);
    ui_toolButtonStyleBox->setShrinkable(true);
    ui_toolButtonStyleBox->addItem("Icon only", Qt::ToolButtonIconOnly);
    ui_toolButtonStyleBox->addItem("Text only", Qt::ToolButtonTextOnly);
    ui_toolButtonStyleBox->addItem("Text under icon", Qt::ToolButtonTextUnderIcon);
    ui_toolButtonStyleBox->addItem("Text beside icon", Qt::ToolButtonTextBesideIcon);
    ui_toolButtonStyleBox->setCurrentData( DuSettings::i()->get(
        DuSettings::UI_ToolButtonStyle
        ) );

    appearanceLayout->addRow(tr("Tool buttons style"), ui_toolButtonStyleBox);

    ui_marginsBox = new DuSpinBox(this);
    ui_marginsBox->setMinimum(0);
    ui_marginsBox->setMaximum(10);
    ui_marginsBox->setSuffix(" pt");
    ui_marginsBox->setValue(
        DuSettings::i()->get(DuSettings::UI_Margins).toInt()
        );
    appearanceLayout->addRow(tr("Spacing"), ui_marginsBox);

    ui_showTrayIconBox = new QCheckBox(tr("Always show"), this);
    ui_showTrayIconBox->setChecked( DuSettings::i()->get(DuSettings::UI_ShowTrayIcon).toBool() );
    appearanceLayout->addRow(tr("Tray icon"), ui_showTrayIconBox);

    ui_trayIconModeBox = new DuComboBox(this);
    ui_trayIconModeBox->addItem(tr("Dark"), "dark");
    ui_trayIconModeBox->addItem(tr("Color"), "color");
    ui_trayIconModeBox->addItem(tr("Light"), "light");
    ui_trayIconModeBox->setCurrentData( DuSettings::i()->get(DuSettings::UI_TrayIconMode).toString() );
    appearanceLayout->addRow(tr("Tray icon color"), ui_trayIconModeBox);

    QWidget *ui_colorsWidget = addSection(tr("Colors"), l);
    auto colorsLayout = qobject_cast<QFormLayout*>( ui_colorsWidget->layout() );

    QColor fgColor = DuSettings::i()->get(
                                        DuSettings::UI_ForegroundColor
                                        ).value<QColor>();
    QColor bgColor = DuSettings::i()->get(
                                        DuSettings::UI_BackgroundColor
                                        ).value<QColor>();

    ui_focusColorSelector = new DuColorSelector(ui_colorsWidget);
    ui_focusColorSelector->setColor( DuSettings::i()->get(
                                                       DuSettings::UI_FocusColor
                                                       ).value<QColor>() );

    colorsLayout->addRow(tr("Focus color"), ui_focusColorSelector);

    ui_iconColorSelector = new DuColorSelector(ui_colorsWidget);
    ui_iconColorSelector->setColor( DuSettings::i()->get(
                                                      DuSettings::UI_IconColor
                                                      ).value<QColor>() );

    colorsLayout->addRow(tr("Icons color"), ui_iconColorSelector);

    ui_fgColorSelector = new DuColorSelector(ui_colorsWidget);
    ui_fgColorSelector->setColor( fgColor );

    colorsLayout->addRow(tr("Foreground color"), ui_fgColorSelector);

    ui_bgColorSelector = new DuColorSelector(ui_colorsWidget);
    ui_bgColorSelector->setColor( bgColor );

    colorsLayout->addRow(tr("Background color"), ui_bgColorSelector);

    ui_contrastBox = new DuComboBox(ui_colorsWidget);
    ui_contrastBox->addItem(tr("Low"), 1);
    ui_contrastBox->addItem(tr("Medium"), 2);
    ui_contrastBox->addItem(tr("High"), 3);
    ui_contrastBox->setCurrentData(
        DuSettings::i()->get(DuSettings::UI_Contrast).toInt()
        );

    colorsLayout->addRow(tr("Contrast"), ui_contrastBox);

    ui_dateFormatBox = new DuComboBox(this);
    QDate date = QDate::currentDate();
    QString f = "yyyy/MM/dd";
    ui_dateFormatBox->addItem(date.toString(f), f);
    f = DATE_DATA_FORMAT;
    ui_dateFormatBox->addItem(date.toString(f), f);
    f = "MM.dd.yyyy";
    ui_dateFormatBox->addItem(date.toString(f), f);
    f = "dd/MM/yyyy";
    ui_dateFormatBox->addItem(date.toString(f), f);
    f = "ddd MMMM d yyyy";
    ui_dateFormatBox->addItem(date.toString(f), f);
    f = "ddd d MMMM yyyy";
    ui_dateFormatBox->addItem(date.toString(f), f);
    ui_dateFormatBox->setCurrentData( _sm->get(DuSettings::UI_DateFormat) );

    appearanceLayout->addRow(tr("Date format"), ui_dateFormatBox);

    ui_timeFormatBox = new DuComboBox(this);
    QTime time = QTime(18, 37, 23);
    f = TIME_DATA_FORMAT;
    ui_timeFormatBox->addItem(time.toString(f), f);
    f = "h:mm:ss ap";
    ui_timeFormatBox->addItem(time.toString(f), f);
    f = "hh:mm";
    ui_timeFormatBox->addItem(time.toString(f), f);
    f = "h:mm ap";
    ui_timeFormatBox->addItem(time.toString(f), f);
    ui_timeFormatBox->setCurrentData( _sm->get(DuSettings::UI_TimeFormat) );

    appearanceLayout->addRow(tr("Time format"), ui_timeFormatBox);

    l->addWidget(new QLabel(tr("Restart the application\nfor the changes to take effect.")));
}

void SettingsDockWidget::setupUpdatesTab()
{
    QWidget *w = addTab(DuIcon(":/icons/update90"), "Updates");
    auto l = qobject_cast<QVBoxLayout*>( w->layout() );

    QWidget *updatesWidget = addSection(tr("Updates"), l);
    auto updatesLayout = qobject_cast<QFormLayout*>( updatesWidget->layout() );

    ui_checkAtStartupBox = new QCheckBox("Check during startup", this);
    ui_checkAtStartupBox->setChecked(_sm->get(DuSettings::APP_CheckUpdates).toBool());
    updatesLayout->addWidget(ui_checkAtStartupBox);

    ui_checkNowButton = new QPushButton("Check now", this);
    ui_checkNowButton->setIcon(DuIcon(":/icons/check-update"));
    updatesLayout->addWidget(ui_checkNowButton);

    l->addStretch();
}

void SettingsDockWidget::setupDaemonTab()
{
    QWidget *w = addTab(DuIcon(":/icons/daemon90"), "Daemon");
    auto l = qobject_cast<QVBoxLayout*>( w->layout() );

    QWidget *daemonWidget = addSection(tr("Ramses daemon"), l);
    auto daemonLayout = qobject_cast<QFormLayout*>( daemonWidget->layout() );

    ui_daemonPortBox = new DuSpinBox(w);
    ui_daemonPortBox->setPrefix("Port: ");
    ui_daemonPortBox->setMinimum(1024);
    ui_daemonPortBox->setMaximum(65535);
    ui_daemonPortBox->setValue(18185);
    daemonLayout->addWidget(ui_daemonPortBox);

    ui_restartDaemonButton = new QPushButton("Restart Daemon");
    daemonLayout->addWidget(ui_restartDaemonButton);

    l->addStretch();
}

void SettingsDockWidget::connectEvents()
{
    connect(_sm, &DuSettings::settingChanged,
            this, &SettingsDockWidget::updateSettings);

    // APEARANCE
    connect(ui_themeBox, QOverload<int>::of(&QComboBox::activated),
            this, &SettingsDockWidget::setTheme);
    connect( ui_toolButtonStyleBox, QOverload<int>::of(&DuComboBox::activated),
            this, [this] (int i) {
                _sm->set( DuSettings::UI_ToolButtonStyle,
                         ui_toolButtonStyleBox->itemData(i).toInt()
                         );
            } );
    connect( ui_trayIconModeBox, &DuComboBox::dataActivated,
            this, [this] (const QVariant &val) {
                _sm->set(DuSettings::UI_TrayIconMode, val);
            });
    connect( ui_showTrayIconBox, &QCheckBox::clicked,
            this, [this] (bool checked) {
                _sm->set(DuSettings::UI_ShowTrayIcon, checked);
            });
    connect(ui_marginsBox, QOverload<int>::of(&DuSpinBox::valueChanged),
            this, [this] (int v) {
                _sm->set(DuSettings::UI_Margins, v);
            });
    connect( ui_focusColorSelector, &DuColorSelector::colorChanged,
            this, [this] (const QColor &color) {
                _sm->set(DuSettings::UI_FocusColor, color);
            });
    connect( ui_iconColorSelector, &DuColorSelector::colorChanged,
            this, [this] (const QColor &color) {
                _sm->set(DuSettings::UI_IconColor, color);
            });
    connect(ui_fgColorSelector, &DuColorSelector::colorChanged,
            this, [this] (const QColor &color) {
                _sm->set(DuSettings::UI_ForegroundColor, color);
            });
    connect(ui_bgColorSelector, &DuColorSelector::colorChanged,
            this, [this] (const QColor &color) {
                _sm->set(DuSettings::UI_BackgroundColor, color);
            });
    connect(ui_contrastBox, QOverload<int>::of(&QComboBox::activated),
            this, [this] (int i) {
                _sm->set(DuSettings::UI_Contrast, ui_contrastBox->itemData(i));
            });
    connect( ui_dateFormatBox, &DuComboBox::dataActivated,
            this, [this] (const QVariant &val) {
                _sm->set(DuSettings::UI_DateFormat, val);
                QString dateTime = val.toString() + " - " +
                                   _sm->get(DuSettings::UI_TimeFormat).toString();
                _sm->set(DuSettings::UI_DateTimeFormat, dateTime);
            });
    connect( ui_timeFormatBox, &DuComboBox::dataActivated,
            this, [this] (const QVariant &val) {
                _sm->set(DuSettings::UI_TimeFormat, val);
                QString dateTime = _sm->get(DuSettings::UI_DateFormat).toString() + " - " +
                                   val.toString();
                _sm->set(DuSettings::UI_DateTimeFormat, dateTime);
            });


    // UPDATES

    connect( ui_checkAtStartupBox, &QCheckBox::clicked,
            this, [this] (bool checked) {
                _sm->set(DuSettings::APP_CheckUpdates, checked);
            });

    connect( ui_checkNowButton, &QPushButton::clicked, this, [] () {
        DuApplication *app = qobject_cast<DuApplication*>(qApp);
        app->checkUpdate(true);
        DuQFUpdateDialog dialog( app->updateInfo() );
        dialog.exec();
    });

    // DAEMON

    connect( ui_daemonPortBox, &DuSpinBox::edited,
            this, [this] () {
                _sm->set(RamSettings::DaemonPort, ui_daemonPortBox->value());
            });

    connect( ui_restartDaemonButton, &QPushButton::clicked, Daemon::instance(), &Daemon::restart );
}

QWidget *SettingsDockWidget::addTab(const DuIcon &icon, const QString &name)
{
    auto *w = new QWidget(ui_tabWidget);
    ui_tabWidget->addTab(w, icon, "");
    ui_tabWidget->setTabToolTip(ui_tabWidget->count()-1, name);

    DuUI::addBoxLayout(Qt::Vertical, w);
    return w;
}

QWidget *SettingsDockWidget::addSection(const QString &name, QBoxLayout *layout)
{
    auto label = new QLabel(this);
    label->setText("<b>" + name + "</b>");
    layout->addWidget( label );

    auto w = new QWidget(this);
    w->setProperty("class", "block");
    layout->addWidget(w);

    DuUI::addFormLayout(w);
    return w;
}

QJsonObject SettingsDockWidget::getTheme(const QString &path) const
{
    QFile f(path);

    if (!f.open(QFile::ReadOnly))
        return QJsonObject();

    auto tdoc = QJsonDocument::fromJson(f.readAll());
    f.close();
    return tdoc.object();
}

void SettingsDockWidget::loadThemes()
{
    QSignalBlocker b(ui_themeBox);
    ui_themeBox->clear();

    ui_themeBox->addItem(tr("Custom"), "");

    QDir d(":/themes");
    const QStringList themes = d.entryList(QDir::Files);
    qDebug() << "Available themes:" << themes;
    for (const auto &theme: themes) {

        QJsonObject tobj = getTheme(":/themes/"+theme);
        if (!tobj.contains("name"))
            continue;

        ui_themeBox->addItem(
            tobj.value("name").toString(),
            ":/themes/"+theme
            );
    }

    ui_themeBox->setCurrentIndex(0);
}

void SettingsDockWidget::setTheme(int t)
{
    // Custom
    if (t == 0) {
        ui_focusColorSelector->setEnabled(true);
        ui_iconColorSelector->setEnabled(true);
        ui_fgColorSelector->setEnabled(true);
        ui_bgColorSelector->setEnabled(true);
        ui_contrastBox->setEnabled(true);
        ui_toolButtonStyleBox->setEnabled(true);
        ui_trayIconModeBox->setEnabled(true);
        ui_showTrayIconBox->setEnabled(true);
        ui_marginsBox->setEnabled(true);
        return;
    }

    ui_focusColorSelector->setEnabled(false);
    ui_iconColorSelector->setEnabled(false);
    ui_fgColorSelector->setEnabled(false);
    ui_bgColorSelector->setEnabled(false);
    ui_contrastBox->setEnabled(false);
    ui_toolButtonStyleBox->setEnabled(false);
    ui_trayIconModeBox->setEnabled(false);
    ui_showTrayIconBox->setEnabled(false);
    ui_marginsBox->setEnabled(false);

    QString theme = ui_themeBox->itemData(t).toString();
    QFile f(theme);
    QJsonObject tobj = getTheme(theme);
    QJsonObject colors = tobj.value("colors").toObject();

    DuSettings::i()->set(
        DuSettings::UI_FocusColor,
        QColor( colors.value("focus").toString("#a526c4") )
        );
    DuSettings::i()->set(
        DuSettings::UI_IconColor,
        QColor( colors.value("icons").toString("#b1b1b1") )
        );
    DuSettings::i()->set(
        DuSettings::UI_ForegroundColor,
        QColor( colors.value("foreground").toString("#b1b1b1") )
        );
    DuSettings::i()->set(
        DuSettings::UI_BackgroundColor,
        QColor( colors.value("background").toString("#333333") )
        );
    DuSettings::i()->set(
        DuSettings::UI_Contrast,
        colors.value("contrast").toInt(1)
        );

    QJsonObject appearance = tobj.value("appearance").toObject();

    DuSettings::i()->set(
        DuSettings::UI_ToolButtonStyle,
        appearance.value("toolButtonStyle").toInt(0)
        );
    DuSettings::i()->set(
        DuSettings::UI_TrayIconMode,
        appearance.value("trayIconMode").toString("color")
        );
    DuSettings::i()->set(
        DuSettings::UI_ShowTrayIcon,
        appearance.value("showTrayIcon").toBool(false)
        );
    DuSettings::i()->set(
        DuSettings::UI_Margins,
        appearance.value("spacing").toInt(3)
        );
}
