#include "consoledockwidget.h"

#include "duapp/dusettings.h"
#include "duwidgets/duicon.h"
#include "duwidgets/dutabwidget.h"
#include "duapp/dusettings.h"
#include "duapp/dulogger.h"
#include "duapp/duui.h"

ConsoleDockWidget::ConsoleDockWidget(QWidget *parent) :
    QFrame(parent)
{
    setupUi();

    int level = DuSettings::i()->get(DuSettings::APP_LogLevel).toInt();
    ui_levelBox->setCurrentData( level );
    setLevel(ui_levelBox->currentIndex());

    connectEvents();
}

void ConsoleDockWidget::clear()
{
    ui_general->clear();
    ui_daemon->clear();
    ui_server->clear();
    ui_db->clear();
}

void ConsoleDockWidget::log(const QString &msg, LogType type, const QString &component)
{
    if (component == "" || component == "Ramses") ui_general->log(msg, type, component);
    else if (component == "API") ui_daemon->log(msg, type, component);
    else if (component == "Client" || component == "Server") ui_server->log(msg, type, component);
    else if (component == "Database-Interface" || component == "Local-Database") ui_db->log(msg, type, component);
}

void ConsoleDockWidget::setLevel(QVariant l)
{
    int i = l.toInt();
    auto level = static_cast<LogType>( ui_levelBox->itemData(i).toInt() );
    ui_general->setLevel(level);
    ui_daemon->setLevel(level);
    ui_server->setLevel(level);
    ui_db->setLevel(level);
    DuSettings::i()->set(DuSettings::APP_LogLevel, level);
}

void ConsoleDockWidget::setupUi()
{
    this->setMinimumWidth(300);

    auto *mainLayout = DuUI::addBoxLayout(Qt::Vertical, this);
    int m = DuSettings::i()->get(DuSettings::UI_Margins).toInt();
    mainLayout->setSpacing(m*2);
    mainLayout->setContentsMargins(m*3,m*3,m*3,m*3);

    ui_levelBox = new DuComboBox();
    ui_levelBox->setProperty("class","duBlock");
    ui_levelBox->addItem("Data", DataLog);
    ui_levelBox->addItem("Debug", DebugLog);
    ui_levelBox->addItem("Information", InformationLog);
    ui_levelBox->addItem("Warning", WarningLog);
    ui_levelBox->addItem("Critical", CriticalLog);
    ui_levelBox->addItem("Fatal", FatalLog);
    mainLayout->addWidget(ui_levelBox);

    DuTabWidget *editWidget = new DuTabWidget(this);
    editWidget->setTabPosition(QTabWidget::West);
    mainLayout->addWidget(editWidget);

    ui_general = new DuConsoleWidget(editWidget);
    ui_general->setLevel(InformationLog);
    editWidget->addTab(ui_general, DuIcon(":/icons/ramses90"), "");
    editWidget->setTabToolTip(0, tr("Ramses"));

    ui_daemon = new DuConsoleWidget(editWidget);
    ui_daemon->setLevel(InformationLog);
    editWidget->addTab(ui_daemon, DuIcon(":/icons/daemon90"), "");
    editWidget->setTabToolTip(1, tr("Ramses API (Daemon)"));

    ui_server = new DuConsoleWidget(editWidget);
    ui_server->setLevel(InformationLog);
    editWidget->addTab(ui_server, DuIcon(":/icons/server90"), "");
    editWidget->setTabToolTip(2, tr("Ramses Server"));

    ui_db = new DuConsoleWidget(editWidget);
    ui_db->setLevel(InformationLog);
    editWidget->addTab(ui_db, DuIcon(":/icons/storage90"), "");
    editWidget->setTabToolTip(3, tr("Local database"));


    ui_clearButton = new QPushButton(tr("Clear"), this);
    ui_clearButton->setProperty("class","duBlock");
    ui_clearButton->setIcon(QIcon(":/icons/clean"));
    mainLayout->addWidget(ui_clearButton);
}

void ConsoleDockWidget::connectEvents()
{
    connect(DuLogger::i(), &DuLogger::newLog, this, &ConsoleDockWidget::log);

    connect(ui_clearButton, &QPushButton::clicked, this, &ConsoleDockWidget::clear);
    connect(ui_levelBox, QOverload<QVariant>::of(&DuComboBox::dataActivated), this, &ConsoleDockWidget::setLevel);
}
