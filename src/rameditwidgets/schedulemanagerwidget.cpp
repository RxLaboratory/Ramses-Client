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
    if (!event->spontaneous())
    {
        ui_titleBar->show();
    }
    QWidget::showEvent(event);
}

void ScheduleManagerWidget::hideEvent(QHideEvent *event)
{
    if (!event->spontaneous())
    {
        ui_titleBar->hide();
    }
    QWidget::hideEvent(event);
}

void ScheduleManagerWidget::projectChanged(RamProject *project)
{

    if (m_project) disconnect(m_project, nullptr, this, nullptr);

    m_project = project;

    if (!project)
    {
        this->setEnabled(false);
        m_schedule->setList(nullptr);
        ui_userMenu->setList(nullptr);
        ui_endDateEdit->setDate(QDate::currentDate());
        ui_stepMenu->setList(nullptr);
        ui_stepContextMenu->setList(nullptr);
        ui_timeRemaining->setText("");
        return;
    }

    m_schedule->setList( project->users() );
    ui_userMenu->setList( project->users() );
    ui_endDateEdit->setDate( project->deadline() );
    ui_stepMenu->setList( project->steps() );
    ui_stepContextMenu->setList(project->steps());

    ui_table->resizeColumnsToContents();
    ui_table->resizeRowsToContents();

    int days = QDate::currentDate().daysTo( project->deadline() );
    ui_timeRemaining->setText("Time remaining: " + QString::number(days) + " days");

    connect (m_project, SIGNAL(changed(RamObject*)),this,SLOT(projectUpdated(RamObject*)));
}

void ScheduleManagerWidget::projectUpdated(RamObject *projObj)
{
    if (!m_project->is(projObj)) return;
    int days = QDate::currentDate().daysTo( m_project->deadline() );
    ui_timeRemaining->setText("Time remaining: " + QString::number(days) + " days");
}

void ScheduleManagerWidget::userChanged(RamUser *user)
{
    if (!user)
    {
        this->setEnabled(false);
        return;
    }
    QSettings *uSettings = Ramses::instance()->currentUser()->userSettings();

    ui_monday->setChecked( uSettings->value("schedule/monday", true).toBool() );
    ui_tuesday->setChecked( uSettings->value("schedule/tuesday", true).toBool() );
    ui_wednesday->setChecked( uSettings->value("schedule/wednesday", true).toBool() );
    ui_thursday->setChecked( uSettings->value("schedule/thursday", true).toBool() );
    ui_friday->setChecked( uSettings->value("schedule/friday", true).toBool() );
    ui_saturday->setChecked( uSettings->value("schedule/saturday", true).toBool() );
    ui_sunday->setChecked( uSettings->value("schedule/sunday", true).toBool() );

    ui_stepContextMenu->setEnabled(user->role() >= RamUser::Lead);
    ui_stepMenu->setEnabled(user->role() >= RamUser::Lead);
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
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
            QDateTime date = QDateTime( index.data(Qt::UserRole+1).toDate() );
#else
            QDateTime date = index.data(Qt::UserRole+1).toDate().startOfDay();
#endif
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

    this->update();
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

void ScheduleManagerWidget::showMonday(bool show)
{
    if (show) m_scheduleFilter->showDay(1);
    else m_scheduleFilter->hideDay(1);

    if(Ramses::instance()->currentUser())
    {
        QSettings *uSettings = Ramses::instance()->currentUser()->userSettings();
        uSettings->setValue("schedule/monday", show);
    }

}

void ScheduleManagerWidget::showTuesday(bool show)
{
    if (show) m_scheduleFilter->showDay(2);
    else m_scheduleFilter->hideDay(2);

    if (Ramses::instance()->currentUser() )
    {
        QSettings *uSettings = Ramses::instance()->currentUser()->userSettings();
        uSettings->setValue("schedule/tuesday", show);
    }
}

void ScheduleManagerWidget::showWednesday(bool show)
{
    if (show) m_scheduleFilter->showDay(3);
    else m_scheduleFilter->hideDay(3);

    if (Ramses::instance()->currentUser() )
    {
        QSettings *uSettings = Ramses::instance()->currentUser()->userSettings();
        uSettings->setValue("schedule/wednesday", show);
    }
}

void ScheduleManagerWidget::showThursday(bool show)
{
    if (show) m_scheduleFilter->showDay(4);
    else m_scheduleFilter->hideDay(4);

    if ( Ramses::instance()->currentUser() )
    {
        QSettings *uSettings = Ramses::instance()->currentUser()->userSettings();
        uSettings->setValue("schedule/thursday", show);
    }
}

void ScheduleManagerWidget::showFriday(bool show)
{
    if (show) m_scheduleFilter->showDay(5);
    else m_scheduleFilter->hideDay(5);

    if ( Ramses::instance()->currentUser() )
    {
        QSettings *uSettings = Ramses::instance()->currentUser()->userSettings();
        uSettings->setValue("schedule/friday", show);
    }
}

void ScheduleManagerWidget::showSaturday(bool show)
{
    if (show) m_scheduleFilter->showDay(6);
    else m_scheduleFilter->hideDay(6);

    if(Ramses::instance()->currentUser() )
    {
        QSettings *uSettings = Ramses::instance()->currentUser()->userSettings();
        uSettings->setValue("schedule/saturday", show);
    }
}

void ScheduleManagerWidget::showSunday(bool show)
{
    if (show) m_scheduleFilter->showDay(7);
    else m_scheduleFilter->hideDay(7);

    if (Ramses::instance()->currentUser() )
    {
        QSettings *uSettings = Ramses::instance()->currentUser()->userSettings();
        uSettings->setValue("schedule/sunday", show);
    }
}

void ScheduleManagerWidget::goTo(QDate date)
{
    if(date < ui_startDateEdit->date())
    {
        ui_startDateEdit->setDate(date);
        return;
    }
    if (date > ui_endDateEdit->date())
    {
        ui_endDateEdit->setDate(date);
        return;
    }

    if (m_scheduleFilter->columnCount() == 0) return;

    // Look for the column
    int col = m_scheduleFilter->columnCount();
    for (int i = 0; i < m_scheduleFilter->columnCount(); i++)
    {
        QDate colDate = m_scheduleFilter->headerData(i, Qt::Horizontal, Qt::UserRole).value<QDate>();
        if (colDate >= date)
        {
            col = i;
            break;
        }
    }

    QModelIndex index = m_scheduleFilter->index( ui_table->rowAt(0), col);
    ui_table->scrollTo( index );
}

void ScheduleManagerWidget::updateCurrentDate()
{
    int col = ui_table->columnAt(50);

    QDate date = m_scheduleFilter->headerData(col, Qt::Horizontal, Qt::UserRole).value<QDate>();
    QSignalBlocker b(ui_goTo);
    ui_goTo->setDate(date);
}

void ScheduleManagerWidget::goToToday()
{
    ui_goTo->setDate(QDate::currentDate());
}

void ScheduleManagerWidget::goToDeadline()
{
    RamProject *project = Ramses::instance()->currentProject();
    if (!project) return;
    ui_goTo->setDate(project->deadline());
}

void ScheduleManagerWidget::goToNextMonth()
{
    int col = ui_table->columnAt( ui_table->width()-100 );

    QDate date = m_scheduleFilter->headerData(col, Qt::Horizontal, Qt::UserRole).value<QDate>();
    ui_goTo->setDate( date.addMonths(1) );
}

void ScheduleManagerWidget::goToPreviousMonth()
{
    ui_goTo->setDate( ui_goTo->date().addMonths(-1) );
}

void ScheduleManagerWidget::contextMenuRequested(QPoint p)
{
    qDebug() << "Context menu called";
    // Call the context menu
    ui_contextMenu->popup(ui_table->viewport()->mapToGlobal(p));
}

void ScheduleManagerWidget::comment()
{
    // Get selection
    QModelIndexList selection = ui_table->selectionModel()->selectedIndexes();
    if (selection.count() == 0) return;

    QList<ScheduleEntryStruct> modifiedEntries;

    bool ok;
    QString text = QInputDialog::getMultiLineText(ui_table, tr("Write a comment"),
                                                  tr("Comment:"), "", &ok);
    if (ok && !text.isEmpty())
    {
        m_dbi->suspend(true);

        for (int i = 0; i < selection.count(); i++)
        {
            const QModelIndex &index = selection.at(i);
            RamScheduleEntry *entry = reinterpret_cast<RamScheduleEntry*>( index.data(Qt::UserRole).toULongLong() );
            if (!entry) continue;
            entry->setComment(text);
            entry->update();
            modifiedEntries << entry->toStruct();
        }

        m_dbi->suspend(false);
        m_dbi->updateSchedules( modifiedEntries );
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
    ui_table->setContextMenuPolicy(Qt::CustomContextMenu);
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

    QMenu *dayMenu = new QMenu(this);

    ui_monday    = new QAction("Monday", this);
    ui_monday->setCheckable(true);
    ui_monday->setChecked(true);
    dayMenu->addAction(ui_monday);
    ui_tuesday   = new QAction("Tuesday", this);
    ui_tuesday->setCheckable(true);
    ui_tuesday->setChecked(true);
    dayMenu->addAction(ui_tuesday);
    ui_wednesday = new QAction("Wednesday", this);
    ui_wednesday->setCheckable(true);
    ui_wednesday->setChecked(ui_wednesday);
    dayMenu->addAction(ui_wednesday);
    ui_thursday  = new QAction("Thursday", this);
    ui_thursday->setCheckable(true);
    ui_thursday->setChecked(true);
    dayMenu->addAction(ui_thursday);
    ui_friday = new QAction("Friday", this);
    ui_friday->setCheckable(true);
    ui_friday->setChecked(true);
    dayMenu->addAction(ui_friday);
    ui_saturday  = new QAction("Saturday", this);
    ui_saturday->setCheckable(true);
    ui_saturday->setChecked(true);
    dayMenu->addAction(ui_saturday);
    ui_sunday    = new QAction("Sunday", this);
    ui_sunday->setCheckable(true);
    ui_sunday->setChecked(true);
    dayMenu->addAction(ui_sunday);

    QToolButton *dayButton  = new QToolButton(this);
    dayButton->setText("Days");
    dayButton->setIcon(QIcon(":/icons/calendar"));
    dayButton->setIconSize(QSize(16,16));
    dayButton->setObjectName("menuButton");
    dayButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    dayButton->setPopupMode(QToolButton::InstantPopup);
    dayButton->setMenu(dayMenu);

    ui_titleBar->insertLeft( dayButton );

    QMenu *stepMenu = new QMenu(this);

    ui_commentAction = new QAction("Add comment...", this);
    stepMenu->addAction(ui_commentAction);

    ui_stepMenu = new RamObjectListMenu(false, this);
    ui_stepMenu->setTitle("Assign");
    ui_stepMenu->addCreateButton();
    ui_stepMenu->actions().at(0)->setText("None");
    stepMenu->addMenu(ui_stepMenu);

    QToolButton *stepButton = new QToolButton(this);
    stepButton->setText("Step");
    stepButton->setIcon(QIcon(":/icons/step"));
    stepButton->setIconSize(QSize(16,16));
    stepButton->setObjectName("menuButton");
    stepButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    stepButton->setPopupMode(QToolButton::InstantPopup);
    stepButton->setMenu(stepMenu);

    ui_titleBar->insertLeft( stepButton );

    QLabel *goToLabel = new QLabel("Go to:", this);
    ui_titleBar->insertLeft(goToLabel);

    ui_goTo = new QDateEdit(this);
    ui_goTo->setCalendarPopup(true);
    ui_goTo->setDate(QDate::currentDate());
    ui_titleBar->insertLeft(ui_goTo);

    ui_today = new QToolButton(this);
    ui_today->setText("Today");
    ui_today->setObjectName("menuButton");
    ui_today->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    ui_titleBar->insertLeft(ui_today);

    ui_deadline = new QToolButton(this);
    ui_deadline->setText("Deadline");
    ui_deadline->setObjectName("menuButton");
    ui_deadline->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    ui_titleBar->insertLeft(ui_deadline);

    ui_prevMonth = new QToolButton(this);
    ui_prevMonth->setText("◀ Previous month");
    ui_prevMonth->setObjectName("menuButton");
    ui_prevMonth->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    ui_titleBar->insertLeft(ui_prevMonth);

    ui_nextMonth = new QToolButton(this);
    ui_nextMonth->setText("Next month ▶");
    ui_nextMonth->setObjectName("menuButton");
    ui_nextMonth->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    ui_titleBar->insertLeft(ui_nextMonth);

    ui_timeRemaining = new QLabel("Time remaining: -- days", this);
    ui_titleBar->insertRight(ui_timeRemaining);

    QLabel *fromLabel = new QLabel("From:", this);
    ui_titleBar->insertRight(fromLabel);

    ui_startDateEdit = new QDateEdit(this);
    ui_startDateEdit->setCalendarPopup(true);
    ui_startDateEdit->setDate(QDate::currentDate().addDays(-5));
    ui_titleBar->insertRight(ui_startDateEdit);

    QLabel *toLabel = new QLabel("To:", this);
    ui_titleBar->insertRight(toLabel);

    ui_endDateEdit = new QDateEdit(this);
    ui_endDateEdit->setCalendarPopup(true);
    ui_endDateEdit->setDate(QDate::currentDate());
    ui_titleBar->insertRight(ui_endDateEdit);

    // Context menu
    ui_contextMenu = new QMenu(this);
    ui_contextMenu->addAction(ui_commentAction);

    ui_stepContextMenu = new RamObjectListMenu(false, this);
    ui_stepContextMenu->setTitle("Assign");
    ui_stepContextMenu->addCreateButton();
    ui_stepContextMenu->actions().at(0)->setText("None");
    ui_contextMenu->addMenu(ui_stepContextMenu);
}

void ScheduleManagerWidget::connectEvents()
{
    // dates
    connect(ui_startDateEdit, SIGNAL(dateChanged(QDate)), m_schedule, SLOT(setStartDate(QDate)));
    connect(ui_endDateEdit, SIGNAL(dateChanged(QDate)), m_schedule, SLOT(setEndDate(QDate)));
    connect(ui_goTo, SIGNAL(dateChanged(QDate)), this, SLOT(goTo(QDate)));
    connect(ui_today,SIGNAL(clicked()),this,SLOT(goToToday()));
    connect(ui_deadline,SIGNAL(clicked()),this,SLOT(goToDeadline()));
    connect(ui_nextMonth,SIGNAL(clicked()),this,SLOT(goToNextMonth()));
    connect(ui_prevMonth,SIGNAL(clicked()),this,SLOT(goToPreviousMonth()));
    connect(ui_table->horizontalScrollBar(),SIGNAL(valueChanged(int)),this,SLOT(updateCurrentDate()));
    // days
    connect(ui_monday,SIGNAL(toggled(bool)),this,SLOT(showMonday(bool)));
    connect(ui_tuesday,SIGNAL(toggled(bool)),this,SLOT(showTuesday(bool)));
    connect(ui_wednesday,SIGNAL(toggled(bool)),this,SLOT(showWednesday(bool)));
    connect(ui_thursday,SIGNAL(toggled(bool)),this,SLOT(showThursday(bool)));
    connect(ui_friday,SIGNAL(toggled(bool)),this,SLOT(showFriday(bool)));
    connect(ui_saturday,SIGNAL(toggled(bool)),this,SLOT(showSaturday(bool)));
    connect(ui_sunday,SIGNAL(toggled(bool)),this,SLOT(showSunday(bool)));
    // users
    connect(ui_userMenu,SIGNAL(assign(RamObject*,bool)), this, SLOT(filterUser(RamObject*,bool)));
    connect(ui_meAction,SIGNAL(triggered()), this, SLOT(filterMe()));
    // batch steps
    connect(ui_stepMenu, SIGNAL(create()), this, SLOT(assignStep()));
    connect(ui_stepMenu, SIGNAL(assign(RamObject*)), this, SLOT(assignStep(RamObject*)));
    connect(ui_stepContextMenu,SIGNAL(create()), this, SLOT(assignStep()));
    connect(ui_stepContextMenu, SIGNAL(assign(RamObject*)), this, SLOT(assignStep(RamObject*)));
    QShortcut *s = new QShortcut(QKeySequence(QKeySequence::Delete), ui_table );
    connect(s, SIGNAL(activated()), this, SLOT(assignStep()));
    // context menu
    connect(ui_table, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenuRequested(QPoint)));
    connect(ui_commentAction, SIGNAL(triggered()), this, SLOT(comment()));
    // other
    connect(ui_titleBar, &TitleBar::closeRequested, this, &ScheduleManagerWidget::closeRequested);
    connect(Ramses::instance(), SIGNAL(currentProjectChanged(RamProject*)), this, SLOT(projectChanged(RamProject*)));
    connect(Ramses::instance(), SIGNAL(loggedIn(RamUser*)), this, SLOT(userChanged(RamUser*)));

}


