#include "itemtablemanagerwidget.h"

ItemTableManagerWidget::ItemTableManagerWidget(RamStep::Type productionType, QWidget *parent) : QWidget(parent)
{   
    setupUi();
    m_stepFilter = new RamStepFilterModel(productionType, this);
    m_productionType = productionType;
    if (m_productionType == RamStep::ShotProduction) ui_table->setSortable(true);
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
    for (int i = 4; i < actions.count(); i++)
    {
        RamStep *step = reinterpret_cast<RamStep*>( actions[i]->data().toULongLong() );
        if (!step) continue;
        RamUser *u = Ramses::instance()->currentUser();
        if (step->users()->contains( u ))
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
    // Check if its a status
    if (obj->objectType() == RamObject::Status)
    {
        RamStatus *status = qobject_cast<RamStatus*>( obj );

        // If it's not the current user, create a new one
        RamUser *currentUser = Ramses::instance()->currentUser();
        if(!status->user()->is(currentUser))
        {
            status = new RamStatus(
                        currentUser,
                        status->state(),
                        status->step(),
                        status->item());
            status->item()->addStatus(status);
        }
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

void ItemTableManagerWidget::setupUi()
{
    // Get the mainwindow to add the titlebar
    QMainWindow *mw = GuiUtils::appMainWindow();
    mw->addToolBarBreak(Qt::TopToolBarArea);

    ui_titleBar = new TitleBar("",false, mw);
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

    // Menus
    ui_stepMenu = new QMenu(this);

    ui_actionSelectAllSteps = new QAction("Select All", this);
    ui_stepMenu->addAction(ui_actionSelectAllSteps);

    ui_actionSelectNoSteps = new QAction("Select None", this);
    ui_stepMenu->addAction(ui_actionSelectNoSteps);

    ui_actionSelectMySteps = new QAction("Select my steps", this);
    ui_stepMenu->addAction(ui_actionSelectMySteps);

    ui_stepMenu->addSeparator();

    QToolButton *stepButton = new QToolButton(this);
    stepButton->setText("Steps");
    stepButton->setIcon(QIcon(":/icons/step"));
    stepButton->setIconSize(QSize(16,16));
    stepButton->setObjectName("menuButton");
    stepButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    stepButton->setPopupMode(QToolButton::InstantPopup);
    stepButton->setMenu(ui_stepMenu);

    ui_titleBar->insertLeft(stepButton);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->setSpacing(3);
    mainLayout->setContentsMargins(0,0,0,0);

    ui_table = new RamObjectListWidget(RamObjectListWidget::Table, this);
    ui_table->setEditableObjects(true, RamUser::ProjectAdmin);
    mainLayout->addWidget(ui_table);

    this->setLayout(mainLayout);
}

void ItemTableManagerWidget::connectEvents()
{
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
