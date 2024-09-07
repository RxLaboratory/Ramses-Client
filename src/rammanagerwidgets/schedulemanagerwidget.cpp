#include "schedulemanagerwidget.h"

#include "duutils/guiutils.h"
#include "duwidgets/duicon.h"
#include "progressmanager.h"
#include "qscrollbar.h"
#include "ramschedulecomment.h"
#include "ramscheduleentry.h"
#include "ramses.h"
#include "progressmanager.h"
#include "ramstep.h"
#include "scheduleentrycreationdialog.h"

ScheduleManagerWidget::ScheduleManagerWidget(QWidget *parent) : QWidget(parent)
{
    m_dbi = DBInterface::i();

    setupUi();
    m_schedule = new RamScheduleTableModel( );

    m_scheduleFilter = new RamScheduleFilterProxyModel();
    m_scheduleFilter->setSourceModel( m_schedule );
    ui_table->setModel( m_scheduleFilter );

    connectEvents();
}

void ScheduleManagerWidget::showEvent(QShowEvent *event)
{
    if (!event->spontaneous())
    {
        ui_titleBar->show();
        changeProject();
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
    RamUser *user = Ramses::i()->currentUser();
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
        ui_rowsMenu->saveState(uSettings, "rows");
        uSettings->endGroup();
    }

    QWidget::hideEvent(event);
}

void ScheduleManagerWidget::checkRowsFilter()
{
    bool ok = ui_rowsMenu->isAllChecked();

    QString t = ui_rowsButton->text().replace(" ▽","");
    if (!ok) t = t + " ▽";
    ui_rowsButton->setText( t );
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

    m_projectChanged = true;

    if (m_project) disconnect(m_project, nullptr, this, nullptr);

    m_project = project;

    // Reload in the show event if not yet visible
    // to improve perf: do not refresh all the app when changing the project, only what's visible.
    if ( this->isVisible() ) changeProject();
}

void ScheduleManagerWidget::projectUpdated(RamObject *projObj)
{
    if (!m_project->is(projObj)) return;
    int days = QDate::currentDate().daysTo( m_project->deadline() );
    ui_timeRemaining->setText("Time remaining: " + QString::number(days) + " days");
}

void ScheduleManagerWidget::ramsesReady()
{
    // Reload settings
    loadSettings();
}

void ScheduleManagerWidget::changeUserRole(RamAbstractObject::UserRole role)
{
    ui_addEntryMenu->setEnabled(role >= RamUser::Lead);
    ui_replaceEntryMenu->setEnabled(role >= RamUser::Lead);
    ui_addRowAction->setEnabled(role >= RamUser::ProjectAdmin);
    ui_removeRowAction->setEnabled(role >= RamUser::ProjectAdmin);
}

void ScheduleManagerWidget::addEntry(RamObject *stepObj)
{
    if (!m_project) return;

    QModelIndexList selection = ui_table->selectionModel()->selectedIndexes();
    int count = selection.count();

    if (count == 0)
        return;

    QString name;
    QString comment;

    if (stepObj) {
        name = stepObj->shortName();
    }
    else {
        ScheduleEntryCreationDialog dialog(this);
        if (dialog.exec()) {
            name = dialog.title();
            comment = dialog.comment();
        }
        if (name == "" && comment == "")
            return;
    }

    m_project->suspendEstimations(true);

    ProgressManager *pm = ProgressManager::instance();
    pm->setTitle(tr("Creating schedule entries"));

    pm->setMaximum(count);
    pm->start();

    for (int i = 0; i < count; i++)
    {
        pm->increment();
        const QModelIndex &index = selection.at(i);

        QString rowUuid = ui_table->model()->headerData(index.row(), Qt::Vertical, RamObject::UUID).toString();
        if (rowUuid == "")
            continue;
        auto scheduleRow = RamScheduleRow::get(rowUuid);
        if (!scheduleRow)
            continue;

        QDate date = index.data(RamObject::Date).toDate();

        auto entry = new RamScheduleEntry(name, date, scheduleRow);
        if (stepObj)
            entry->setStep(stepObj);
        else
            entry->setComment(comment);
    }

    pm->finish();
    this->update();

    m_project->suspendEstimations(false);
}

bool ScheduleManagerWidget::clearSelectedEntries()
{
    if (!m_project) return false;

    const QModelIndexList selection = ui_table->selectionModel()->selectedIndexes();
    int count = selection.count();

    if (count == 0)
        return false;

    bool needConfirm = false;
    for (const auto &index: selection) {
        const QVector<RamScheduleEntry*> entries = RamScheduleEntry::get(index);
        if (entries.isEmpty())
            continue;
        needConfirm = true;
        break;
    }

    if ( needConfirm && QMessageBox::question(
        this,
        tr("Confirm deletion"),
        tr("Are you sure you want to remove all selected entries?")
        ) !=  QMessageBox::Yes)
        return false;

    m_project->suspendEstimations(true);

    for (const auto &index: selection) {
        const QVector<RamScheduleEntry*> entries = RamScheduleEntry::get(index);
        for (auto entry: entries) {
            entry->remove();
        }
    }

    m_project->suspendEstimations(false);

    return true;
}

void ScheduleManagerWidget::filterUser(RamObject *user, bool filter)
{
    if (filter) m_scheduleFilter->acceptUserUuid( user->uuid() );
    else m_scheduleFilter->ignoreUserUuid( user->uuid() );

    checkRowsFilter();
}

void ScheduleManagerWidget::filterMe()
{  
    QList<QAction*> actions = ui_rowsMenu->actions();
    RamUser *u = Ramses::i()->currentUser();

    for (int i = 4; i < actions.count(); i++)
    {
        auto scheduleRow = RamScheduleRow::c( ui_rowsMenu->objectAt(i) );
        if (!scheduleRow)
            continue;

        RamUser *user = scheduleRow->user();

        if (u->is(user))
            actions[i]->setChecked(true);
        else
            actions[i]->setChecked(false);
    }

    checkRowsFilter();
}

void ScheduleManagerWidget::addRow()
{
    RamProject *project = Ramses::i()->project();
    if (!project) return;

    QString rowNum = QString::number(
        project->scheduleRows()->count() + 1
        );

    auto row = new RamScheduleRow(
        rowNum,
        tr("Row %1").arg(rowNum),
        project
        );

    row->edit();
}

void ScheduleManagerWidget::removeSelectedRows()
{
    RamProject *project = Ramses::i()->project();
    if (!project) return;

    const QModelIndexList selection = ui_table->selectionModel()->selectedIndexes();

    if (selection.count() == 0) return;

    QMessageBox::StandardButton confirm = QMessageBox::question(
        this,
        "Confirm deletion",
        "Are you sure you want to premanently remove the selected rows?"
        );

    if ( confirm != QMessageBox::Yes) return;

    QSet<RamScheduleRow*> rows;
    for (const auto index: selection)
        rows << m_schedule->scheduleRow(index);

    for (auto row: qAsConst(rows)) {
        qDebug() << "Removing schedule row" << row->name();
        row->remove();
    }
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
        QDate colDate = m_scheduleFilter->headerData(i, Qt::Horizontal, RamObject::Date).value<QDate>();
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

    QDate date = m_scheduleFilter->headerData(col, Qt::Horizontal, RamObject::Date).value<QDate>();
    QSignalBlocker b(ui_goTo);
    ui_goTo->setDate(date);
}

void ScheduleManagerWidget::goToToday()
{
    ui_goTo->setDate(QDate::currentDate());
}

void ScheduleManagerWidget::goToDeadline()
{
    RamProject *project = Ramses::i()->project();
    if (!project) return;
    ui_goTo->setDate(project->deadline());
    goTo(ui_goTo->date());
}

void ScheduleManagerWidget::goToNextMonth()
{
    ui_goTo->setDate( ui_goTo->date().addMonths(1) );
    goTo(ui_goTo->date());
}

void ScheduleManagerWidget::goToPreviousMonth()
{
    ui_goTo->setDate( ui_goTo->date().addMonths(-1) );
}

void ScheduleManagerWidget::copyEntries()
{
    m_entryClipBoard = ui_table->selectionModel()->selectedIndexes();
    m_clipboardAction = Qt::CopyAction;
}

void ScheduleManagerWidget::cutEntries()
{
    m_entryClipBoard = ui_table->selectionModel()->selectedIndexes();
    m_clipboardAction = Qt::MoveAction;
}

void ScheduleManagerWidget::pasteEntries()
{
    if (m_entryClipBoard.isEmpty())
        return;

    // Get the offset
    QModelIndex newIndex = ui_table->selectionModel()->currentIndex();
    QModelIndex oldIndex = m_entryClipBoard.first();

    int rowOffset = newIndex.row() - oldIndex.row();
    int dayOffset = newIndex.column() - oldIndex.column();

    m_project->suspendEstimations(true);

    for (const auto &index: m_entryClipBoard) {

        const QVector<RamScheduleEntry*> entries = RamScheduleEntry::get(index);
        if (entries.isEmpty())
            continue;

        RamScheduleEntry *testEntry = entries.first();

        RamScheduleRow *oldRow = testEntry->row();
        int oldRowIndex = m_project->scheduleRows()->objectRow(oldRow);
        int newRowIndex = oldRowIndex + rowOffset;
        if (newRowIndex < 0 || newRowIndex >= m_project->scheduleRows()->count())
            continue;

        RamObject *newRow = m_project->scheduleRows()->get(newRowIndex);
        QDate newDate = testEntry->date().addDays(dayOffset);

        for (auto entry: entries) {
            // Move: update the row and date
            if (m_clipboardAction == Qt::MoveAction) {
                entry->setRow(newRow);
                entry->setDate(newDate);
            }
            else if (m_clipboardAction == Qt::CopyAction){
                auto newEntry = new RamScheduleEntry(
                    entry->name(),
                    newDate,
                    RamScheduleRow::c(newRow)
                    );
                newEntry->setStep(entry->step());
                newEntry->setColor(entry->color());
                newEntry->setComment(entry->comment());
            }
        }
    }

    m_project->suspendEstimations(false);

    this->update();
}

void ScheduleManagerWidget::contextMenuRequested(QPoint p)
{
    QModelIndex currentIndex = ui_table->selectionModel()->currentIndex();
    if ( !currentIndex.isValid() ) return;

    ui_entriesMenu->popup(ui_table->viewport()->mapToGlobal(p));
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

    ui_table = new RamScheduleTableView(this);
    mainLayout->addWidget(ui_table);

    this->setLayout(mainLayout);

    // Title bar

    ui_actionShowDetails = new QAction(" " + tr("Details"), this);
    ui_actionShowDetails->setIcon(DuIcon(":/icons/show"));
    ui_actionShowDetails->setCheckable(true);

    QToolButton *detailsButton = new QToolButton(this);
    detailsButton->setDefaultAction(ui_actionShowDetails);
    detailsButton->setIconSize(QSize(16,16));
    detailsButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    ui_titleBar->insertLeft(detailsButton);

    ui_rowsMenu = new RamObjectMenu(true, this);

    ui_meAction = new QAction("Me", this);
    ui_rowsMenu->insertAction( ui_rowsMenu->actions().at(0), ui_meAction);

    ui_rowsMenu->insertSeparator( ui_rowsMenu->actions().at(0) );

    ui_removeRowAction = new QAction(DuIcon(":/icons/remove"), tr("Remove row"), this);
    ui_rowsMenu->insertAction(ui_rowsMenu->actions().at(0), ui_removeRowAction);

    ui_addRowAction = new QAction(DuIcon(":/icons/add"), tr("Add row"), this);
    ui_rowsMenu->insertAction(ui_rowsMenu->actions().at(0), ui_addRowAction);

    ui_rowsButton = new QToolButton(this);
    ui_rowsButton->setText("Rows");
    ui_rowsButton->setIcon(DuIcon(":/icons/user"));
    ui_rowsButton->setIconSize(QSize(16,16));
    ui_rowsButton->setObjectName("menuButton");
    ui_rowsButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    ui_rowsButton->setPopupMode(QToolButton::InstantPopup);
    ui_rowsButton->setMenu(ui_rowsMenu);

    ui_titleBar->insertLeft( ui_rowsButton );

    DuMenu *dayMenu = new DuMenu(this);

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
    dayButton->setIcon(DuIcon(":/icons/calendar"));
    dayButton->setIconSize(QSize(16,16));
    dayButton->setObjectName("menuButton");
    dayButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    dayButton->setPopupMode(QToolButton::InstantPopup);
    dayButton->setMenu(dayMenu);

    ui_titleBar->insertLeft( dayButton );

    // Context/Entries menu
    ui_entriesMenu = new DuMenu(this);

    ui_addEntryMenu = new RamObjectMenu(false, this);
    ui_addEntryMenu->setTitle("Add entry");
    ui_addEntryMenu->addCreateButton();
    ui_addEntryMenu->actions().at(0)->setText("Generic entry");
    ui_entriesMenu->addMenu(ui_addEntryMenu);

    ui_replaceEntryMenu = new RamObjectMenu(false, this);
    ui_replaceEntryMenu->setTitle("Replace entries");
    ui_replaceEntryMenu->addCreateButton();
    ui_replaceEntryMenu->actions().at(0)->setText("Generic entry");
    ui_entriesMenu->addMenu(ui_replaceEntryMenu);

    ui_clearAction = new QAction("Clear", this);
    ui_clearAction->setIcon(DuIcon(":/icons/trash"));
    ui_entriesMenu->addAction(ui_clearAction);

    ui_entriesMenu->addSeparator();

    ui_copyEntries = new QAction(DuIcon(":/icons/copy"), tr("Copy entries"), this);
    ui_copyEntries->setShortcut(QKeySequence::Copy);

    ui_cutEntries = new QAction(DuIcon(":/icons/cut"), tr("Cut entries"), this);
    ui_cutEntries->setShortcut(QKeySequence::Cut);

    ui_pasteEntries = new QAction(DuIcon(":/icons/paste"), tr("Paste entries"), this);
    ui_pasteEntries->setShortcut(QKeySequence::Paste);

    ui_entriesMenu->addAction(ui_copyEntries);
    ui_entriesMenu->addAction(ui_cutEntries);
    ui_entriesMenu->addAction(ui_pasteEntries);

    ui_entriesMenu->addSeparator();

    QToolButton *entriesButton = new QToolButton(this);
    entriesButton->setText("Schedule entry");
    entriesButton->setIcon(DuIcon(":/icons/step"));
    entriesButton->setIconSize(QSize(16,16));
    entriesButton->setObjectName("menuButton");
    entriesButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    entriesButton->setPopupMode(QToolButton::InstantPopup);
    entriesButton->setMenu(ui_entriesMenu);

    ui_titleBar->insertLeft( entriesButton );

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
    connect(ui_table->horizontalScrollBar(),&QScrollBar::valueChanged,this,&ScheduleManagerWidget::updateCurrentDate);
    // days
    connect(ui_monday,SIGNAL(toggled(bool)),this,SLOT(showMonday(bool)));
    connect(ui_tuesday,SIGNAL(toggled(bool)),this,SLOT(showTuesday(bool)));
    connect(ui_wednesday,SIGNAL(toggled(bool)),this,SLOT(showWednesday(bool)));
    connect(ui_thursday,SIGNAL(toggled(bool)),this,SLOT(showThursday(bool)));
    connect(ui_friday,SIGNAL(toggled(bool)),this,SLOT(showFriday(bool)));
    connect(ui_saturday,SIGNAL(toggled(bool)),this,SLOT(showSaturday(bool)));
    connect(ui_sunday,SIGNAL(toggled(bool)),this,SLOT(showSunday(bool)));
    // users & rows
    connect(ui_rowsMenu,SIGNAL(assignmentChanged(RamObject*,bool)), this, SLOT(filterUser(RamObject*,bool)));
    connect(ui_meAction,SIGNAL(triggered()), this, SLOT(filterMe()));
    connect(ui_addRowAction, &QAction::triggered, this, &ScheduleManagerWidget::addRow);
    connect(ui_removeRowAction, &QAction::triggered, this, &ScheduleManagerWidget::removeSelectedRows);
    // batch steps
    connect(ui_addEntryMenu, &RamObjectMenu::createTriggered,
            this, [this] () { addEntry(); });
    connect(ui_addEntryMenu, &RamObjectMenu::assigned, this, &ScheduleManagerWidget::addEntry);
    connect(ui_replaceEntryMenu, &RamObjectMenu::createTriggered,
            this, [this] () {
        if (clearSelectedEntries())
            addEntry();
    });
    connect(ui_replaceEntryMenu, &RamObjectMenu::assigned,
            this, [this] (RamObject *o) {
        if (clearSelectedEntries())
            addEntry(o);
    });
    connect(ui_clearAction, &QAction::triggered, this, &ScheduleManagerWidget::clearSelectedEntries);
    // context menu
    connect(ui_table, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenuRequested(QPoint)));
    // comment actions
    connect(ui_copyEntries, &QAction::triggered, this, &ScheduleManagerWidget::copyEntries);
    connect(ui_cutEntries, &QAction::triggered, this, &ScheduleManagerWidget::cutEntries);
    connect(ui_pasteEntries, &QAction::triggered, this, &ScheduleManagerWidget::pasteEntries);
    // other actions
    // other
    connect(ui_titleBar, &DuQFTitleBar::closeRequested, this, &ScheduleManagerWidget::closeRequested);
    connect(Ramses::i(), SIGNAL(currentProjectChanged(RamProject*)), this, SLOT(projectChanged(RamProject*)));
    connect(Ramses::i(), &Ramses::ready, this, &ScheduleManagerWidget::ramsesReady);
    connect(Ramses::i(), &Ramses::roleChanged, this, &ScheduleManagerWidget::changeUserRole);

    connect(m_schedule, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(usersInserted(QModelIndex,int,int)));
}

void ScheduleManagerWidget::loadSettings()
{
    RamUser *u = Ramses::i()->currentUser();
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
    ui_rowsMenu->restoreState(uSettings, "rows");
    uSettings->endGroup();
}

void ScheduleManagerWidget::changeProject()
{
    if (!m_projectChanged) return;
    m_projectChanged = false;

    if (!m_project)
    {
        this->setEnabled(false);
        m_schedule->setObjectModel(nullptr, nullptr);
        ui_rowsMenu->setObjectModel(nullptr);
        ui_endDateEdit->setDate(QDate::currentDate());
        ui_addEntryMenu->setObjectModel(nullptr);
        ui_replaceEntryMenu->setObjectModel(nullptr);
        ui_timeRemaining->setText("");
        return;
    }
    this->setEnabled(true);

    m_schedule->setObjectModel( m_project->scheduleRows(), m_project->scheduleEntries() );

    ui_rowsMenu->setObjectModel( m_project->scheduleRows() );
    ui_endDateEdit->setDate( QDate::currentDate().addDays(30) );
    ui_addEntryMenu->setObjectModel(m_project->steps());
    ui_replaceEntryMenu->setObjectModel(m_project->steps());

    //ui_table->resizeColumnsToContents();
    ui_table->resizeRowsToContents();

    int days = QDate::currentDate().daysTo( m_project->deadline() );
    ui_timeRemaining->setText("Time remaining: " + QString::number(days) + " days");

    loadSettings();

    connect (m_project, &RamProject::dataChanged,this, &ScheduleManagerWidget::projectUpdated);//*/
}
