#include "schedulemanagerwidget.h"

#include "duqf-utils/guiutils.h"
#include "ramscheduleentry.h"
#include "ramschedulecomment.h"
#include "ramses.h"

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

    // Save filters and layout
    RamUser *user = Ramses::instance()->currentUser();
    if (user)
    {
        QSettings *uSettings = user->settings();

        uSettings->beginGroup("schedule");
        // Days of the week
        uSettings->setValue("monday", ui_monday->isChecked());
        uSettings->setValue("tuesday", ui_tuesday->isChecked());
        uSettings->setValue("wednesday", ui_wednesday->isChecked());
        uSettings->setValue("thursday", ui_thursday->isChecked());
        uSettings->setValue("friday", ui_friday->isChecked());
        uSettings->setValue("saturday", ui_saturday->isChecked());
        uSettings->setValue("sunday", ui_sunday->isChecked());
        // Show details
        uSettings->setValue("showDetails", ui_actionShowDetails->isChecked());
        // User filters
        ui_userMenu->saveState(uSettings, "users");
        uSettings->endGroup();
    }

    QWidget::hideEvent(event);
}

void ScheduleManagerWidget::checkUserFilter()
{
    bool ok = ui_userMenu->isAllChecked();

    QString t = ui_userButton->text().replace(" ▽","");
    if (!ok) t = t + " ▽";
    ui_userButton->setText( t );
}

void ScheduleManagerWidget::usersInserted(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent);
    for( int i = first; i <= last; i++)
    {
        ui_table->resizeRowToContents(i);
    }
}

void ScheduleManagerWidget::projectChanged(RamProject *project)
{
    if (!m_project && !project) return;
    if (m_project) if (m_project->is(project) ) return;

    if (m_project)
    {
        disconnect(m_project, nullptr, this, nullptr);
    }

    m_project = project;

    if (!project)
    {
        this->setEnabled(false);
        m_schedule->setList(nullptr, nullptr);
        ui_userMenu->setList(nullptr);
        ui_endDateEdit->setDate(QDate::currentDate());
        ui_stepMenu->setList(nullptr);
        ui_stepContextMenu->setList(nullptr);
        ui_timeRemaining->setText("");
        return;
    }
    this->setEnabled(true);

    m_schedule->setList( project->users(), project->scheduleComments() );
    ui_userMenu->setList( project->users() );
    ui_endDateEdit->setDate( project->deadline() );
    ui_stepMenu->setList( project->steps() );
    ui_stepContextMenu->setList(project->steps());

    ui_table->resizeColumnsToContents();
    ui_table->resizeRowsToContents();

    int days = QDate::currentDate().daysTo( project->deadline() );
    ui_timeRemaining->setText("Time remaining: " + QString::number(days) + " days");

    loadSettings();

    connect (m_project, &RamProject::dataChanged,this, &ScheduleManagerWidget::projectUpdated);//*/
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

    // Reload settings
    loadSettings();

    ui_stepContextMenu->setEnabled(user->role() >= RamUser::Lead);
    ui_stepMenu->setEnabled(user->role() >= RamUser::Lead);
}

void ScheduleManagerWidget::assignStep(RamObject *step)
{
    QModelIndexList selection = ui_table->selectionModel()->selectedIndexes();
    for (int i = 0; i < selection.count(); i++)
    {
        const QModelIndex &index = selection.at(i);
        const quintptr &iptr = index.data(RamObjectList::Pointer).toULongLong();
        RamScheduleEntry *entry = reinterpret_cast<RamScheduleEntry*>( iptr );
        RamScheduleComment *comment = reinterpret_cast<RamScheduleComment*>( iptr );

        if (!step)
        {
            if (index.data(RamObjectList::IsComment).toBool() && comment) {
                comment->remove();
            }
            else if (entry)
            {
                entry->remove();
            }
            continue;
        }

        if (!entry)
        {
            RamUser *user = reinterpret_cast<RamUser*>(
                        ui_table->selectionModel()->model()->headerData( index.row(), Qt::Vertical, RamObjectList::Pointer ).toULongLong() );
            if (!user) continue;
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
            QDateTime date = QDateTime( index.data(RamObjectList::Date).toDate() );
#else
            QDateTime date = index.data(RamObjectList::Date).toDate().startOfDay();
#endif
            bool ispm = ui_table->selectionModel()->model()->headerData( index.row(), Qt::Vertical, RamObjectList::IsPM ).toBool();
            if ( ispm )
                date.setTime(QTime(12,0));

            entry = new RamScheduleEntry( user, date );
            entry->setStep( RamStep::c(step) );
            user->schedule()->append(entry);
        }
        else
        {
            entry->setStep( RamStep::c(step) );
        }

    }
}

void ScheduleManagerWidget::filterUser(RamObject *user, bool filter)
{
    if (filter) m_scheduleFilter->acceptUserUuid( user->uuid() );
    else m_scheduleFilter->ignoreUserUuid( user->uuid() );

    checkUserFilter();
}

void ScheduleManagerWidget::filterMe()
{
    QList<QAction*> actions = ui_userMenu->actions();

    RamUser *current = Ramses::instance()->currentUser();
    ui_userMenu->select(current);
    checkUserFilter();
}

void ScheduleManagerWidget::showMonday(bool show)
{
    if (show) m_scheduleFilter->showDay(1);
    else m_scheduleFilter->hideDay(1);
}

void ScheduleManagerWidget::showTuesday(bool show)
{
    if (show) m_scheduleFilter->showDay(2);
    else m_scheduleFilter->hideDay(2);
}

void ScheduleManagerWidget::showWednesday(bool show)
{
    if (show) m_scheduleFilter->showDay(3);
    else m_scheduleFilter->hideDay(3);
}

void ScheduleManagerWidget::showThursday(bool show)
{
    if (show) m_scheduleFilter->showDay(4);
    else m_scheduleFilter->hideDay(4);
}

void ScheduleManagerWidget::showFriday(bool show)
{
    if (show) m_scheduleFilter->showDay(5);
    else m_scheduleFilter->hideDay(5);
}

void ScheduleManagerWidget::showSaturday(bool show)
{
    if (show) m_scheduleFilter->showDay(6);
    else m_scheduleFilter->hideDay(6);
}

void ScheduleManagerWidget::showSunday(bool show)
{
    if (show) m_scheduleFilter->showDay(7);
    else m_scheduleFilter->hideDay(7);
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
        QDate colDate = m_scheduleFilter->headerData(i, Qt::Horizontal, RamObjectList::Date).value<QDate>();
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

    QDate date = m_scheduleFilter->headerData(col, Qt::Horizontal, RamObjectList::Date).value<QDate>();
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

    QDate date = m_scheduleFilter->headerData(col, Qt::Horizontal, RamObjectList::Date).value<QDate>();
    ui_goTo->setDate( date.addMonths(1) );
}

void ScheduleManagerWidget::goToPreviousMonth()
{
    ui_goTo->setDate( ui_goTo->date().addMonths(-1) );
}

void ScheduleManagerWidget::copyComment()
{
    QModelIndex currentIndex = ui_table->selectionModel()->currentIndex();
    if ( !currentIndex.isValid() ) return;

    const quintptr &iptr = currentIndex.data(RamObjectList::Pointer).toULongLong();
    if (iptr == 0) return;
    if (currentIndex.data(RamObjectList::IsComment).toBool()) {
        RamScheduleComment *comment = reinterpret_cast<RamScheduleComment*>( iptr );
        if (!comment) return;
        QClipboard *clipboard = QGuiApplication::clipboard();
        clipboard->setText( comment->comment() );
    }
    else {
        RamScheduleEntry *entry = reinterpret_cast<RamScheduleEntry*>( iptr );
        if (!entry) return;
        QClipboard *clipboard = QGuiApplication::clipboard();
        clipboard->setText( entry->comment() );
    }
}

void ScheduleManagerWidget::cutComment()
{
    QModelIndex currentIndex = ui_table->selectionModel()->currentIndex();
    if ( !currentIndex.isValid() ) return;

    const quintptr &iptr = currentIndex.data(RamObjectList::Pointer).toULongLong();
    if (iptr == 0) return;
    if (currentIndex.data(RamObjectList::IsComment).toBool()) {
        RamScheduleComment *comment = reinterpret_cast<RamScheduleComment*>( iptr );
        if (!comment) return;
        QClipboard *clipboard = QGuiApplication::clipboard();
        clipboard->setText( comment->comment() );
        comment->setComment("");
    }
    else {
        RamScheduleEntry *entry = reinterpret_cast<RamScheduleEntry*>( iptr );
        if (!entry) return;
        QClipboard *clipboard = QGuiApplication::clipboard();
        clipboard->setText( entry->comment() );
        entry->setComment("");
    }
}

void ScheduleManagerWidget::pasteComment()
{
    QClipboard *clipboard = QGuiApplication::clipboard();
    QString comment = clipboard->text();
    if (comment == "") return;

    // Get selection
    QModelIndexList selection = ui_table->selectionModel()->selectedIndexes();
    if (selection.count() == 0) return;

    for (int i = 0; i < selection.count(); i++)
    {
        const QModelIndex &index = selection.at(i);
        const quintptr &iptr = index.data(RamObjectList::Pointer).toULongLong();
        if ( index.data(RamObjectList::IsComment).toBool() )
        {
            RamScheduleComment *c = reinterpret_cast<RamScheduleComment*>( iptr );
            if (!c) continue;
            c->setComment(comment);
        }
        else
        {
            RamScheduleEntry *entry = reinterpret_cast<RamScheduleEntry*>( iptr );
            if (!entry) continue;
            entry->setComment(comment);
        }
    }
}

void ScheduleManagerWidget::contextMenuRequested(QPoint p)
{
    // If it's a comment row, adjust menu
    QModelIndex currentIndex = ui_table->selectionModel()->currentIndex();
    if ( !currentIndex.isValid() ) return;

    // Call the right context menu
    if( currentIndex.data(RamObjectList::IsComment).toBool() ) ui_commentContextMenu->popup(ui_table->viewport()->mapToGlobal(p));
    else ui_contextMenu->popup(ui_table->viewport()->mapToGlobal(p));
}

void ScheduleManagerWidget::comment()
{
    // Get selection
    QModelIndexList selection = ui_table->selectionModel()->selectedIndexes();
    if (selection.count() == 0) return;

    QString currentComment;
    QModelIndex currentIndex = ui_table->selectionModel()->currentIndex();
    if ( currentIndex.isValid() )
    {
        const quintptr &iptr = currentIndex.data(RamObjectList::Pointer).toULongLong();
        if (currentIndex.data(RamObjectList::IsComment).toBool()) {
            RamScheduleComment *comment = reinterpret_cast<RamScheduleComment*>( iptr );
            if (comment) currentComment = comment->comment();
        }
        else {
            RamScheduleEntry *entry = reinterpret_cast<RamScheduleEntry*>( iptr );
            if (entry) currentComment = entry->comment();
        }

    }

    bool ok;
    QString text = QInputDialog::getMultiLineText(ui_table, tr("Write a comment"),
                                                  tr("Comment:"), currentComment, &ok);
    if (ok && !text.isEmpty())
    {
        for (int i = 0; i < selection.count(); i++)
        {
            const QModelIndex &index = selection.at(i);
            const quintptr &iptr = index.data(RamObjectList::Pointer).toULongLong();
            if (index.data(RamObjectList::IsComment).toBool()) {
                RamScheduleComment *comment = reinterpret_cast<RamScheduleComment*>( iptr );
                if (!comment) {

#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
                    QDateTime date = QDateTime( index.data(RamObjectList::Date).toDate() );
#else
                    QDateTime date = index.data(RamObjectList::Date).toDate().startOfDay();
#endif
                    bool ispm = ui_table->selectionModel()->model()->headerData( index.row(), Qt::Vertical, RamObjectList::IsPM ).toBool();
                    if ( ispm )
                        date.setTime(QTime(12,0));

                    comment = new RamScheduleComment( m_project );
                    comment->setDate(date);
                    m_project->scheduleComments()->append(comment);
                }
                comment->setComment(text);
            }
            else {
                RamScheduleEntry *entry = reinterpret_cast<RamScheduleEntry*>( iptr );
                if (!entry) continue;
                entry->setComment(text);
            }
        }
    }
}

void ScheduleManagerWidget::removeCommment()
{

}

void ScheduleManagerWidget::color()
{
    // Get selection
    QModelIndexList selection = ui_table->selectionModel()->selectedIndexes();
    if (selection.count() == 0) return;

    QColor currentColor;

    QModelIndex currentIndex = ui_table->selectionModel()->currentIndex();
    if ( currentIndex.isValid() )
    {
        const quintptr &iptr = currentIndex.data(RamObjectList::Pointer).toULongLong();
        if (currentIndex.data(RamObjectList::IsComment).toBool()) {
            RamScheduleComment *comment = reinterpret_cast<RamScheduleComment*>( iptr );
            if (comment) currentColor = comment->color();
        }
    }

    QColor color = QColorDialog::getColor(currentColor, this, "Select color" );
    if (color.isValid()) {

        for (int i = 0; i < selection.count(); i++)
        {
            const QModelIndex &index = selection.at(i);
            const quintptr &iptr = index.data(RamObjectList::Pointer).toULongLong();
            if (index.data(RamObjectList::IsComment).toBool()) {
                RamScheduleComment *comment = reinterpret_cast<RamScheduleComment*>( iptr );
                if (!comment) {
                    comment = new RamScheduleComment( m_project );
                    comment->setDate(index.data(RamObjectList::Date).toDateTime());
                    m_project->scheduleComments()->append(comment);
                }
                comment->setColor(color);
            }
        }
    }
}

void ScheduleManagerWidget::setupUi()
{
    // Get the mainwindow to add the titlebar
    QMainWindow *mw = GuiUtils::appMainWindow();
    mw->addToolBarBreak(Qt::TopToolBarArea);

    ui_titleBar = new DuQFTitleBar("Schedule",false, mw);
    ui_titleBar->setObjectName("scheduleToolBar");
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

    QMenu *viewMenu = new QMenu(this);

    ui_actionShowDetails = new QAction("Show details", this);
    ui_actionShowDetails->setCheckable(true);
    viewMenu->addAction(ui_actionShowDetails);

    QToolButton *viewButton = new QToolButton(this);
    viewButton->setText(" View");
    viewButton->setIcon(QIcon(":/icons/show"));
    viewButton->setMenu(viewMenu);
    viewButton->setIconSize(QSize(16,16));
    viewButton->setObjectName("menuButton");
    viewButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    viewButton->setPopupMode(QToolButton::InstantPopup);
    ui_titleBar->insertLeft(viewButton);

    ui_userMenu = new RamObjectListMenu(true, this);

    ui_meAction = new QAction("Me", this);
    ui_userMenu->insertAction( ui_userMenu->actions().at(0), ui_meAction);

    ui_userButton = new QToolButton(this);
    ui_userButton->setText("Users");
    ui_userButton->setIcon(QIcon(":/icons/user"));
    ui_userButton->setIconSize(QSize(16,16));
    ui_userButton->setObjectName("menuButton");
    ui_userButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    ui_userButton->setPopupMode(QToolButton::InstantPopup);
    ui_userButton->setMenu(ui_userMenu);

    ui_titleBar->insertLeft( ui_userButton );

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

    ui_commentAction = new QAction(QIcon(":/icons/comment"), "Comment...", this);
    stepMenu->addAction(ui_commentAction);

    stepMenu->addSeparator();

    ui_copyComment = new QAction("Copy comment", this);
    ui_copyComment->setShortcut(QKeySequence("Ctrl+C"));
    ui_copyComment->setIcon(QIcon(":/icons/copy"));
    stepMenu->addAction(ui_copyComment);

    ui_cutComment = new QAction("Cut comment", this);
    ui_cutComment->setShortcut(QKeySequence("Ctrl+X"));
    ui_cutComment->setIcon(QIcon(":/icons/cut"));
    stepMenu->addAction(ui_cutComment);

    ui_pasteComment = new QAction("Paste as comment", this);
    ui_pasteComment->setShortcut(QKeySequence("Ctrl+V"));
    ui_pasteComment->setIcon(QIcon(":/icons/paste"));
    stepMenu->addAction(ui_pasteComment);

    stepMenu->addSeparator();

    ui_stepMenu = new RamObjectListMenu(false, this);
    ui_stepMenu->setTitle("Assign");
    ui_stepMenu->addCreateButton();
    ui_stepMenu->actions().at(0)->setText("None");
    stepMenu->addMenu(ui_stepMenu);

    QToolButton *stepButton = new QToolButton(this);
    stepButton->setText("Schedule entry");
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

    ui_contextMenu->addSeparator();

    ui_contextMenu->addAction(ui_copyComment);
    ui_contextMenu->addAction(ui_cutComment);
    ui_contextMenu->addAction(ui_pasteComment);

    ui_contextMenu->addSeparator();

    ui_stepContextMenu = new RamObjectListMenu(false, this);
    ui_stepContextMenu->setTitle("Assign");
    ui_stepContextMenu->addCreateButton();
    ui_stepContextMenu->actions().at(0)->setText("None");
    ui_contextMenu->addMenu(ui_stepContextMenu);

    // Comment context menu
    ui_commentContextMenu = new QMenu(this);
    ui_commentContextMenu->addAction(ui_commentAction);

    ui_colorAction = new QAction(QIcon(":/icons/color"), "Color...", this);
    ui_commentContextMenu->addAction(ui_colorAction);

    ui_commentContextMenu->addSeparator();

    ui_commentContextMenu->addAction(ui_copyComment);
    ui_commentContextMenu->addAction(ui_cutComment);
    ui_commentContextMenu->addAction(ui_pasteComment);

    ui_commentContextMenu->addSeparator();

    ui_removeCommentAction = new QAction(QIcon(":/icons/remove"), "Remove", this);
    ui_commentContextMenu->addAction(ui_removeCommentAction);

}

void ScheduleManagerWidget::connectEvents()
{
    // view
    connect(ui_actionShowDetails, SIGNAL(toggled(bool)), ui_table, SLOT(showDetails(bool)));
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
    connect(ui_userMenu,SIGNAL(assignmentChanged(RamObject*,bool)), this, SLOT(filterUser(RamObject*,bool)));
    connect(ui_meAction,SIGNAL(triggered()), this, SLOT(filterMe()));
    // batch steps
    connect(ui_stepMenu, SIGNAL(createTriggered()), this, SLOT(assignStep()));
    connect(ui_stepMenu, SIGNAL(assigned(RamObject*)), this, SLOT(assignStep(RamObject*)));
    connect(ui_stepContextMenu,SIGNAL(createTriggered()), this, SLOT(assignStep()));
    connect(ui_stepContextMenu, SIGNAL(assigned(RamObject*)), this, SLOT(assignStep(RamObject*)));
    QShortcut *s = new QShortcut(QKeySequence(QKeySequence::Delete), ui_table );
    connect(s, SIGNAL(activated()), this, SLOT(assignStep()));
    // context menu
    connect(ui_table, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenuRequested(QPoint)));
    connect(ui_commentAction, SIGNAL(triggered()), this, SLOT(comment()));
    connect(ui_colorAction, SIGNAL(triggered()), this, SLOT(color()));
    connect(ui_removeCommentAction, SIGNAL(triggered()), this, SLOT(assignStep()));
    // comment actions
    connect(ui_copyComment, SIGNAL(triggered()), this, SLOT(copyComment()));
    connect(ui_cutComment, SIGNAL(triggered()), this, SLOT(cutComment()));
    connect(ui_pasteComment, SIGNAL(triggered()), this, SLOT(pasteComment()));
    // other
    connect(ui_titleBar, &DuQFTitleBar::closeRequested, this, &ScheduleManagerWidget::closeRequested);
    connect(Ramses::instance(), SIGNAL(currentProjectChanged(RamProject*)), this, SLOT(projectChanged(RamProject*)));
    connect(Ramses::instance(), &Ramses::userChanged, this, &ScheduleManagerWidget::userChanged);

    connect(m_schedule, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(usersInserted(QModelIndex,int,int)));
}

void ScheduleManagerWidget::loadSettings()
{
    RamUser *u = Ramses::instance()->currentUser();
    if (!u) return;
    QSettings *uSettings = u->settings();
    uSettings->beginGroup("schedule");
    // Days
    ui_monday->setChecked( uSettings->value("monday", true).toBool() );
    ui_tuesday->setChecked( uSettings->value("tuesday", true).toBool() );
    ui_wednesday->setChecked( uSettings->value("wednesday", true).toBool() );
    ui_thursday->setChecked( uSettings->value("thursday", true).toBool() );
    ui_friday->setChecked( uSettings->value("friday", true).toBool() );
    ui_saturday->setChecked( uSettings->value("saturday", true).toBool() );
    ui_sunday->setChecked( uSettings->value("sunday", true).toBool() );
    // Details
    ui_actionShowDetails->setChecked( uSettings->value("showDetails", true).toBool() );
    // Users
    ui_userMenu->restoreState(uSettings, "users");
    uSettings->endGroup();
}


