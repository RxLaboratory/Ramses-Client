#include "itemtablemanagerwidget.h"

ItemTableManagerWidget::ItemTableManagerWidget(RamStep::Type productionType, QWidget *parent) : QWidget(parent)
{   
    setupUi();
    m_stepFilter = new RamStepFilterModel(productionType, this);
    m_productionType = productionType;
    if (m_productionType == RamStep::ShotProduction)
    {
        ui_table->setSortable(true);
        ui_titleBar->setTitle("Shots");
    }
    connectEvents();
}

void ItemTableManagerWidget::selectAllSteps()
{
    QList<QAction*> actions = ui_stepMenu->actions();
    for (int i = 4; i < actions.count(); i++)
    {
        actions[i]->setChecked(true);
    }
}

void ItemTableManagerWidget::selectUserSteps()
{
    QList<QAction*> actions = ui_stepMenu->actions();
    RamUser *u = Ramses::instance()->currentUser();

    for (int i = 4; i < actions.count(); i++)
    {
        RamStep *step = reinterpret_cast<RamStep*>( actions[i]->data().toULongLong() );
        if (!step) continue;
        if (u->isStepAssigned(step))
            actions[i]->setChecked(true);
        else
            actions[i]->setChecked(false);
    }
}

void ItemTableManagerWidget::deselectSteps()
{
    QList<QAction*> actions = ui_stepMenu->actions();
    for (int i = 4; i < actions.count(); i++)
    {
        actions[i]->setChecked(false);
    }
}

void ItemTableManagerWidget::showEvent(QShowEvent *event)
{
    if (!event->spontaneous()) ui_titleBar->show();
    QWidget::showEvent(event);
}

void ItemTableManagerWidget::hideEvent(QHideEvent *event)
{
    if (!event->spontaneous())
    {
        ui_titleBar->hide();
    }
    QWidget::hideEvent(event);
}

void ItemTableManagerWidget::projectChanged(RamProject *project)
{
    this->setEnabled(false );

    // Clear step list
    QList<QAction*> actions = ui_stepMenu->actions();
    for (int i = actions.count() -1; i >= 4; i--)
    {
        actions.at(i)->deleteLater();
    }


    m_project = project;

    if(!project)
    {
        ui_table->setList(nullptr);
        return;
    }

    // Populate list and table
    m_stepFilter->setList(project->steps());
    if(m_stepFilter->rowCount() > 0)
        addStep(QModelIndex(), 0, m_stepFilter->rowCount()-1 );

    if (m_productionType == RamStep::AssetProduction)
    {
        ui_table->setList( project->assets() );
        ui_groupBox->setList( project->assetGroups() );
    }
    else if (m_productionType == RamStep::ShotProduction)
    {
        ui_table->setList( project->shots() );
        ui_groupBox->setList( project->sequences() );
    }

    ui_assignUserMenu->setList(project->users());

    this->setEnabled(true);
}

void ItemTableManagerWidget::addStep(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent)

    for(int i = first; i <= last; i++)
    {
        quintptr iptr = m_stepFilter->data(m_stepFilter->index(i,0), Qt::UserRole).toULongLong();
        RamStep *step = reinterpret_cast<RamStep*>( iptr );
        QAction *stepAction = new QAction(step->name(), this);
        stepAction->setCheckable(true);
        stepAction->setData( iptr );
        stepAction->setChecked(true);
        ui_stepMenu->addAction(stepAction);
        connect (stepAction, SIGNAL(toggled(bool)), this, SLOT(stepActionToggled(bool)));
    }
}

void ItemTableManagerWidget::removeStep(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent)

    for(int i = first; i <= last; i++)
    {
        quintptr iptr = m_stepFilter->data(m_stepFilter->index(i,0), Qt::UserRole).toULongLong();
        QList<QAction*> actions = ui_stepMenu->actions();
        for (int i = 4; i < actions.count(); i++)
        {
            if (actions[i]->data().toULongLong() == iptr)
            {
                actions[i]->deleteLater();
                return;
            }
        }
    }

}

void ItemTableManagerWidget::stepChanged(const QModelIndex &first, const QModelIndex &last, QVector<int> roles)
{
    Q_UNUSED(roles)

    for(int i = first.row(); i <= last.row(); i++)
    {
        quintptr iptr = m_stepFilter->data(m_stepFilter->index(i,0), Qt::UserRole).toULongLong();
        QList<QAction*> actions = ui_stepMenu->actions();
        for (int i = 4; i < actions.count(); i++)
        {
            if (actions[i]->data().toULongLong() == iptr)
            {
                RamStep *step = reinterpret_cast<RamStep*>( iptr );
                actions[i]->setText(step->name());
                return;
            }
        }
    }
}

void ItemTableManagerWidget::stepActionToggled(bool checked)
{
    if(!m_project) return;
    QAction *action = (QAction*)sender();
    quintptr iptr = action->data().toULongLong();
    RamStep *step = reinterpret_cast<RamStep*>( iptr );
    if(checked)
    {
        if (m_productionType == RamStep::AssetProduction) m_project->assets()->removeStepFilter(step);
        else if (m_productionType == RamStep::ShotProduction) m_project->shots()->removeStepFilter(step);
    }
    else
    {
        if (m_productionType == RamStep::AssetProduction) m_project->assets()->addStepFilter(step);
        else if (m_productionType == RamStep::ShotProduction) m_project->shots()->addStepFilter(step);
    }
    ui_table->resizeColumnsToContents();
}

void ItemTableManagerWidget::editObject(RamObject *obj) const
{
    // Check if it's a status
    if (obj->objectType() == RamObject::Status)
    {
        RamStatus *status = qobject_cast<RamStatus*>( obj );

        // If it's not the current user, create a new one
        RamUser *currentUser = Ramses::instance()->currentUser();
        if(!status->user()->is(currentUser))
            status = RamStatus::copy( status, currentUser );

        status->edit();
        return;
    }
    obj->edit();
}

void ItemTableManagerWidget::historyObject(RamObject *obj) const
{
    // get the step history
    RamStatus *status = qobject_cast<RamStatus*>(obj);
    RamStep *step = status->step();
    RamItem *item = status->item();
    item->statusHistory(step)->edit();
}

void ItemTableManagerWidget::unassignUser()
{
    QList<RamStatus*> status = beginEditSelectedStatus();
    for (int i = 0; i < status.count(); i++)
    {
        status.at(i)->assignUser(nullptr);
        status.at(i)->update();
    }
}

void ItemTableManagerWidget::assignUser(RamObject *usrObj)
{
    RamUser *user = qobject_cast<RamUser*>( usrObj );
    if (!user) return;

    QList<RamStatus*> status = beginEditSelectedStatus();
    for (int i = 0; i < status.count(); i++)
    {
        status.at(i)->assignUser(user);
        status.at(i)->update();
    }
}

void ItemTableManagerWidget::changeState(RamObject *sttObj)
{
    RamState *stt = qobject_cast<RamState*>( sttObj );
    if (!stt) return;

    QList<RamStatus*> status = beginEditSelectedStatus();
    for (int i = 0; i < status.count(); i++)
    {
        status.at(i)->setState(stt);
        status.at(i)->update();
    }
}

void ItemTableManagerWidget::setVeryEasy()
{
    setDiffculty(RamStatus::VeryEasy);
}

void ItemTableManagerWidget::setEasy()
{
    setDiffculty(RamStatus::Easy);
}

void ItemTableManagerWidget::setMedium()
{
    setDiffculty(RamStatus::Medium);
}

void ItemTableManagerWidget::setHard()
{
    setDiffculty(RamStatus::Hard);
}

void ItemTableManagerWidget::setVeryHard()
{
    setDiffculty(RamStatus::VeryHard);
}

void ItemTableManagerWidget::setDiffculty(RamStatus::Difficulty difficulty)
{
    QList<RamStatus*> status = beginEditSelectedStatus();
    for (int i = 0; i < status.count(); i++)
    {
        status.at(i)->setDifficulty( difficulty );
        status.at(i)->update();
    }
}

void ItemTableManagerWidget::setCompletion()
{
    QAction* action = qobject_cast<QAction*>( sender() );
    int completion = action->data().toInt();
    QList<RamStatus*> status = beginEditSelectedStatus();
    for (int i = 0; i < status.count(); i++)
    {
        status.at(i)->setCompletionRatio( completion );
        status.at(i)->update();
    }
}

void ItemTableManagerWidget::setupUi()
{
    // Get the mainwindow to add the titlebar
    QMainWindow *mw = GuiUtils::appMainWindow();
    mw->addToolBarBreak(Qt::TopToolBarArea);

    ui_titleBar = new TitleBar("Assets",false, mw);
    ui_titleBar->showReinitButton(false);
    mw->addToolBar(Qt::TopToolBarArea,ui_titleBar);
    ui_titleBar->setFloatable(false);
    ui_titleBar->hide();

    // group box
    ui_groupBox = new RamObjectListComboBox(true,this);
    ui_titleBar->insertLeft(ui_groupBox);

    // Search field
    ui_searchEdit = new DuQFSearchEdit(this);
    ui_searchEdit->setMaximumWidth(150);
    ui_searchEdit->hideSearchButton();
    ui_titleBar->insertLeft(ui_searchEdit);

    // View Menu
    QMenu *viewMenu = new QMenu(this);

    ui_actionTimeTracking = new QAction("Show time tracking", this);
    ui_actionTimeTracking->setCheckable(true);
    ui_actionTimeTracking->setChecked(true);
    viewMenu->addAction(ui_actionTimeTracking);

    ui_actionCompletionRatio = new QAction("Show completion", this);
    ui_actionCompletionRatio->setCheckable(true);
    ui_actionCompletionRatio->setChecked(true);
    viewMenu->addAction(ui_actionCompletionRatio);

    QToolButton *viewButton = new QToolButton(this);
    viewButton->setText(" View");
    viewButton->setIcon(QIcon(":/icons/show"));
    viewButton->setMenu(viewMenu);
    viewButton->setIconSize(QSize(16,16));
    viewButton->setObjectName("menuButton");
    viewButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    viewButton->setPopupMode(QToolButton::InstantPopup);
    ui_titleBar->insertLeft(viewButton);

    // Step Menu
    ui_stepMenu = new QMenu(this);

    ui_actionSelectAllSteps = new QAction("Select All", this);
    ui_stepMenu->addAction(ui_actionSelectAllSteps);

    ui_actionSelectNoSteps = new QAction("Select None", this);
    ui_stepMenu->addAction(ui_actionSelectNoSteps);

    ui_actionSelectMySteps = new QAction("Select my steps", this);
    ui_stepMenu->addAction(ui_actionSelectMySteps);

    ui_stepMenu->addSeparator();

    QToolButton *stepButton = new QToolButton(this);
    stepButton->setText(" Steps");
    stepButton->setIcon(QIcon(":/icons/step"));
    stepButton->setIconSize(QSize(16,16));
    stepButton->setObjectName("menuButton");
    stepButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    stepButton->setPopupMode(QToolButton::InstantPopup);
    stepButton->setMenu(ui_stepMenu);

    ui_titleBar->insertLeft(stepButton);

    // Status menu
    QMenu *statusMenu = new QMenu(this);

    ui_assignUserMenu = new RamObjectListMenu(false, this);
    ui_assignUserMenu->setTitle("Assign user");
    ui_assignUserMenu->addCreateButton();
    ui_assignUserMenu->actions().at(0)->setText("None");
    statusMenu->addMenu(ui_assignUserMenu);

    ui_changeStateMenu = new RamObjectListMenu(false, this);
    ui_changeStateMenu->setTitle("Change state");
    ui_changeStateMenu->setList(Ramses::instance()->states());
    statusMenu->addMenu(ui_changeStateMenu);

    ui_changeDifficultyMenu = new QMenu("Change difficulty", this);
    ui_veryEasy = new QAction("Very easy", this);
    ui_easy = new QAction("Easy", this);
    ui_medium = new QAction("Medium", this);
    ui_hard = new QAction("Hard", this);
    ui_veryHard = new QAction("Very hard", this);
    ui_changeDifficultyMenu->addAction(ui_veryEasy);
    ui_changeDifficultyMenu->addAction(ui_easy);
    ui_changeDifficultyMenu->addAction(ui_medium);
    ui_changeDifficultyMenu->addAction(ui_hard);
    ui_changeDifficultyMenu->addAction(ui_veryHard);
    statusMenu->addMenu(ui_changeDifficultyMenu);

    QMenu *completionMenu = new QMenu("Set completion", this);
    ui_completion0   = new QAction("0%",this);
    ui_completion10  = new QAction("10%",this);
    ui_completion25  = new QAction("25%",this);
    ui_completion50  = new QAction("50%",this);
    ui_completion75  = new QAction("75%",this);
    ui_completion90  = new QAction("90%",this);
    ui_completion100 = new QAction("100%",this);
    ui_completion0->setData(0);
    ui_completion10->setData(10);
    ui_completion25->setData(25);
    ui_completion50->setData(50);
    ui_completion75->setData(75);
    ui_completion90->setData(90);
    ui_completion100->setData(100);
    completionMenu->addAction(ui_completion0  );
    completionMenu->addAction(ui_completion10 );
    completionMenu->addAction(ui_completion25 );
    completionMenu->addAction(ui_completion50 );
    completionMenu->addAction(ui_completion75 );
    completionMenu->addAction(ui_completion90 );
    completionMenu->addAction(ui_completion100);
    statusMenu->addMenu(completionMenu);


    QToolButton *statusButton = new QToolButton(this);
    statusButton->setText(" Status");
    statusButton->setIcon(QIcon(":/icons/state-l"));
    statusButton->setIconSize(QSize(16,16));
    statusButton->setObjectName("menuButton");
    statusButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    statusButton->setPopupMode(QToolButton::InstantPopup);
    statusButton->setMenu(statusMenu);

    ui_titleBar->insertLeft(statusButton);


    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->setSpacing(3);
    mainLayout->setContentsMargins(0,0,0,0);

    ui_table = new RamObjectListWidget(RamObjectListWidget::Table, this);
    ui_table->setEditableObjects(true, RamUser::ProjectAdmin);
    ui_header = new RamStepHeaderView(ui_table);
    ui_table->setHorizontalHeader( ui_header );
    ui_table->setSelectionMode(QAbstractItemView::ExtendedSelection);
    mainLayout->addWidget(ui_table);

    this->setLayout(mainLayout);
}

void ItemTableManagerWidget::connectEvents()
{
    // Status actions
    connect(ui_assignUserMenu,SIGNAL(create()),this,SLOT(unassignUser()));
    connect(ui_assignUserMenu,SIGNAL(assign(RamObject*)),this,SLOT(assignUser(RamObject*)));
    connect(ui_changeStateMenu,SIGNAL(assign(RamObject*)),this,SLOT(changeState(RamObject*)));
    connect(ui_veryEasy,SIGNAL(triggered()),this,SLOT(setVeryEasy()));
    connect(ui_easy,SIGNAL(triggered()),this,SLOT(setEasy()));
    connect(ui_medium,SIGNAL(triggered()),this,SLOT(setMedium()));
    connect(ui_hard,SIGNAL(triggered()),this,SLOT(setHard()));
    connect(ui_veryHard,SIGNAL(triggered()),this,SLOT(setVeryHard()));
    connect(ui_completion0  , SIGNAL(triggered()), this, SLOT( setCompletion() ) );
    connect(ui_completion10 , SIGNAL(triggered()), this, SLOT( setCompletion() ) );
    connect(ui_completion25 , SIGNAL(triggered()), this, SLOT( setCompletion() ) );
    connect(ui_completion50 , SIGNAL(triggered()), this, SLOT( setCompletion() ) );
    connect(ui_completion75 , SIGNAL(triggered()), this, SLOT( setCompletion() ) );
    connect(ui_completion90 , SIGNAL(triggered()), this, SLOT( setCompletion() ) );
    connect(ui_completion100, SIGNAL(triggered()), this, SLOT( setCompletion() ) );
    // view actions
    connect(ui_actionTimeTracking, SIGNAL(triggered(bool)), ui_table, SLOT(setTimeTracking(bool)));
    connect(ui_actionCompletionRatio, SIGNAL(triggered(bool)), ui_table, SLOT(setCompletionRatio(bool)));
    connect(ui_actionTimeTracking, SIGNAL(triggered(bool)), ui_header, SLOT(setTimeTracking(bool)));
    connect(ui_actionCompletionRatio, SIGNAL(triggered(bool)), ui_header, SLOT(setCompletionRatio(bool)));
    // step actions
    connect(ui_actionSelectAllSteps, SIGNAL(triggered()), this, SLOT(selectAllSteps()));
    connect(ui_actionSelectNoSteps, SIGNAL(triggered()), this, SLOT(deselectSteps()));
    connect(ui_actionSelectMySteps, SIGNAL(triggered()), this, SLOT(selectUserSteps()));
    // cell buttons
    connect(ui_table, SIGNAL(editObject(RamObject*)), this, SLOT(editObject(RamObject*)));
    connect(ui_table, SIGNAL(historyObject(RamObject*)), this, SLOT(historyObject(RamObject*)));
    // search
    connect(ui_searchEdit, SIGNAL(changing(QString)), ui_table, SLOT(search(QString)));
    connect(ui_searchEdit, SIGNAL(changed(QString)), ui_table, SLOT(search(QString)));
    // group filter
    connect(ui_groupBox, SIGNAL(currentObjectChanged(RamObject*)), ui_table, SLOT(filter(RamObject*)));
    // step filter
    connect(m_stepFilter, SIGNAL(rowsInserted(QModelIndex,int,int)),this,SLOT(addStep(QModelIndex,int,int)));
    connect(m_stepFilter, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)),this,SLOT(removeStep(QModelIndex,int,int)));
    connect(m_stepFilter, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)), this, SLOT(stepChanged(QModelIndex,QModelIndex,QVector<int>)));
    // other
    connect(ui_titleBar, &TitleBar::closeRequested, this, &ItemTableManagerWidget::closeRequested);
    connect(Ramses::instance(), &Ramses::currentProjectChanged, this, &ItemTableManagerWidget::projectChanged);

}

QList<RamStatus *> ItemTableManagerWidget::beginEditSelectedStatus()
{
    QList<RamStatus*> statuses;
    RamUser *currentUser = Ramses::instance()->currentUser();
    QModelIndexList selection = ui_table->selectionModel()->selectedIndexes();
    for (int i = 0; i < selection.count(); i++)
    {
        const QModelIndex &index = selection.at(i);
        const quintptr &iptr = index.data(Qt::UserRole).toULongLong();
        if (iptr == 0) continue;
        RamObject *obj = reinterpret_cast<RamObject*>( iptr );
        if (obj->objectType() != RamObject::Status) continue;
        RamStatus *status = qobject_cast<RamStatus*>(obj);
        if (!status->user()->is( currentUser ))
            status = RamStatus::copy( status, currentUser );
        statuses << status;
    }
    return statuses;
}
