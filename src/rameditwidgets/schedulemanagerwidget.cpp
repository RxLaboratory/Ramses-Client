#include "schedulemanagerwidget.h"

ScheduleManagerWidget::ScheduleManagerWidget(QWidget *parent) : QWidget(parent)
{
    m_dbi = DBInterface::instance();

    setupUi();
    m_schedule = new RamScheduleTable( );

    m_scheduleFilter = new RamScheduleFilter();
    m_scheduleFilter->setList( m_schedule );
    ui_table->setModel( m_scheduleFilter );

    connectEvents();

}

void ScheduleManagerWidget::showEvent(QShowEvent *event)
{
    if (!event->spontaneous()) ui_titleBar->show();
    QWidget::showEvent(event);
}

void ScheduleManagerWidget::hideEvent(QHideEvent *event)
{
    if (!event->spontaneous()) ui_titleBar->hide();
    QWidget::hideEvent(event);
}

void ScheduleManagerWidget::projectChanged(RamProject *project)
{
    m_schedule->setList( Ramses::instance()->users() );
    ui_userMenu->setList(Ramses::instance()->users());
    ui_endDateEdit->setDate( project->deadline() );
    ui_stepMenu->setList( project->steps() );
}

void ScheduleManagerWidget::assignStep(RamObject *stepObj)
{
    RamStep *step = qobject_cast<RamStep*>(stepObj);

    if (Ramses::instance()->currentProject()->steps()->count() == 0) step = nullptr;

    m_dbi->suspend(true);

    QList<ScheduleEntryStruct> modifiedEntries;
    QList<ScheduleEntryStruct> newEntries;
    QList<ScheduleEntryStruct> removedEntries;

    QModelIndexList selection = ui_table->selectionModel()->selectedIndexes();
    for (int i = 0; i < selection.count(); i++)
    {
        const QModelIndex &index = selection.at(i);
        RamScheduleEntry *entry = reinterpret_cast<RamScheduleEntry*>( index.data(Qt::UserRole).toULongLong() );

        if (!step)
        {
            if (entry)
            {
                removedEntries << entry->toStruct();
                entry->remove(false);
            }
            continue;
        }

        if (!entry)
        {
            RamUser *user = reinterpret_cast<RamUser*>(
                        m_schedule->headerData( index.row(), Qt::Vertical, Qt::UserRole ).toULongLong() );
            if (!user) continue;

            QDateTime date = index.data(Qt::UserRole+1).toDate().startOfDay();
            if ( m_schedule->headerData( index.row(), Qt::Vertical, Qt::UserRole+1 ).toBool() )
                date.setTime(QTime(12,0));

            entry = new RamScheduleEntry( user, step, date );
            user->schedule()->append(entry);

            newEntries << entry->toStruct();
        }
        else
        {
            entry->setStep( step );
            entry->update();
            modifiedEntries << entry->toStruct();
        }

    }

    m_dbi->suspend(false);

    m_dbi->createSchedules( newEntries );
    m_dbi->updateSchedules( modifiedEntries );
    m_dbi->removeSchedules( removedEntries );
}

void ScheduleManagerWidget::filterUser(RamObject *userObj, bool filter)
{
    if (filter) m_scheduleFilter->acceptUserUuid( userObj->uuid() );
    else m_scheduleFilter->ignoreUserUuid( userObj->uuid() );
}

void ScheduleManagerWidget::filterMe()
{
    QList<QAction*> actions = ui_userMenu->actions();

    RamUser *current = Ramses::instance()->currentUser();
    for (int i =0; i < actions.count(); i++)
    {
        QAction *a = actions.at(i);
        quintptr iptr = a->data().toULongLong();
        if (iptr == 0) continue;
        RamUser *u = reinterpret_cast<RamUser*>( iptr );
        if (!u) continue;
        a->setChecked( u->is(current) );
    }
}

void ScheduleManagerWidget::setupUi()
{
    // Get the mainwindow to add the titlebar
    QMainWindow *mw = GuiUtils::appMainWindow();
    mw->addToolBarBreak(Qt::TopToolBarArea);

    ui_titleBar = new TitleBar("Schedule",false, mw);
    ui_titleBar->showReinitButton(false);
    mw->addToolBar(Qt::TopToolBarArea,ui_titleBar);
    ui_titleBar->setFloatable(false);
    ui_titleBar->hide();

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->setSpacing(3);
    mainLayout->setContentsMargins(0,0,0,0);

    ui_table = new RamScheduleTableWidget(this);
    mainLayout->addWidget(ui_table);

    this->setLayout(mainLayout);

    // Title bar

    ui_userMenu = new RamObjectListMenu(true, this);

    ui_meAction = new QAction("Me", this);
    ui_userMenu->insertAction( ui_userMenu->actions().at(0), ui_meAction);

    QToolButton *userButton = new QToolButton(this);
    userButton->setText("Users");
    userButton->setIcon(QIcon(":/icons/user"));
    userButton->setIconSize(QSize(16,16));
    userButton->setObjectName("menuButton");
    userButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    userButton->setPopupMode(QToolButton::InstantPopup);
    userButton->setMenu(ui_userMenu);

    ui_titleBar->insertLeft( userButton );

    ui_stepMenu = new RamObjectListMenu(false, this);
    ui_stepMenu->addCreateButton();
    ui_stepMenu->actions().at(0)->setText("None");

    QToolButton *stepButton = new QToolButton(this);
    stepButton->setText("Steps");
    stepButton->setIcon(QIcon(":/icons/step"));
    stepButton->setIconSize(QSize(16,16));
    stepButton->setObjectName("menuButton");
    stepButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    stepButton->setPopupMode(QToolButton::InstantPopup);
    stepButton->setMenu(ui_stepMenu);

    ui_titleBar->insertLeft( stepButton );

    QLabel *fromLabel = new QLabel("From:", this);
    ui_titleBar->insertRight(fromLabel);

    ui_startDateEdit = new QDateEdit(this);
    ui_startDateEdit->setCalendarPopup(true);
    ui_startDateEdit->setDate(QDate::currentDate());
    ui_titleBar->insertRight(ui_startDateEdit);

    QLabel *toLabel = new QLabel("To:", this);
    ui_titleBar->insertRight(toLabel);

    ui_endDateEdit = new QDateEdit(this);
    ui_endDateEdit->setCalendarPopup(true);
    ui_endDateEdit->setDate(QDate::currentDate());
    ui_titleBar->insertRight(ui_endDateEdit);
}

void ScheduleManagerWidget::connectEvents()
{
    // dates
    connect(ui_startDateEdit, SIGNAL(dateChanged(QDate)), m_schedule, SLOT(setStartDate(QDate)));
    connect(ui_endDateEdit, SIGNAL(dateChanged(QDate)), m_schedule, SLOT(setEndDate(QDate)));
    // users
    connect(ui_userMenu,SIGNAL(assign(RamObject*,bool)), this, SLOT(filterUser(RamObject*,bool)));
    connect(ui_meAction,SIGNAL(triggered()), this, SLOT(filterMe()));
    // batch steps
    connect(ui_stepMenu, SIGNAL(create()), this, SLOT(assignStep()));
    connect(ui_stepMenu, SIGNAL(assign(RamObject*)), this, SLOT(assignStep(RamObject*)));
    // other
    connect(ui_titleBar, &TitleBar::closeRequested, this, &ScheduleManagerWidget::closeRequested);
    connect(Ramses::instance(), SIGNAL(currentProjectChanged(RamProject*)), this, SLOT(projectChanged(RamProject*)));
}


