#include "itemtablemanagerwidget.h"

ItemTableManagerWidget::ItemTableManagerWidget(QString title, QWidget *parent) : QWidget(parent)
{   
    setupUi(title);
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
    // TODO pointer as data instead of uuid
    QList<QAction*> actions = ui_stepMenu->actions();
    for (int i = 4; i < actions.count(); i++)
    {
        RamStep *step = RamStep::step( actions[i]->data().toString() );
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

void ItemTableManagerWidget::stepAdded(RamStep *step)
{
    // TODO pointer as data instead of uuid
    QAction *stepAction = new QAction(step->name(), this);
    stepAction->setCheckable(true);
    stepAction->setData(step->uuid());
    stepAction->setChecked(true);
    ui_stepMenu->addAction(stepAction);
    connect (stepAction, SIGNAL(toggled(bool)), this, SLOT(stepActionToggled(bool)));
}

void ItemTableManagerWidget::stepRemoved(RamObject *stepObj)
{
    // TODO pointer as data instead of uuid
    QList<QAction*> actions = ui_stepMenu->actions();
    for (int i = 4; i < actions.count(); i++)
    {
        QString stepUuid = actions[i]->data().toString();
        if (stepUuid == stepObj->uuid())
        {
            actions[i]->deleteLater();
            return;
        }
    }
}

void ItemTableManagerWidget::stepActionToggled(bool checked)
{
    QAction *action = (QAction*)sender();
    //m_table->setStepVisible( action->data().toString(), checked);
}

void ItemTableManagerWidget::setupUi(QString title)
{
    // Get the mainwindow to add the titlebar
    QMainWindow *mw = GuiUtils::appMainWindow();
    mw->addToolBarBreak(Qt::TopToolBarArea);

    ui_titleBar = new TitleBar(title,false, mw);
    ui_titleBar->showReinitButton(false);
    mw->addToolBar(Qt::TopToolBarArea,ui_titleBar);
    ui_titleBar->setFloatable(false);
    ui_titleBar->hide();

    // Search field
    ui_searchEdit = new DuQFSearchEdit(this);
    ui_searchEdit->setMaximumWidth(150);
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

    ui_table = new RamObjectListWidget(this);
    mainLayout->addWidget(ui_table);

    this->setLayout(mainLayout);
}

void ItemTableManagerWidget::connectEvents()
{
    connect(ui_actionSelectAllSteps, SIGNAL(triggered()), this, SLOT(selectAllSteps()));
    connect(ui_actionSelectNoSteps, SIGNAL(triggered()), this, SLOT(deselectSteps()));
    connect(ui_actionSelectMySteps, SIGNAL(triggered()), this, SLOT(selectUserSteps()));
    /*connect(m_searchEdit, &DuQFSearchEdit::changing, m_table, &ItemTableWidget::search);
    connect(m_searchEdit, &DuQFSearchEdit::changed, m_table, &ItemTableWidget::search);
    connect(m_table, &ItemTableWidget::newStep, this, &ItemTable::stepAdded);
    connect(m_table, &ItemTableWidget::stepRemoved, this, &ItemTable::stepRemoved);*/
    connect(ui_titleBar, &TitleBar::closeRequested, this, &ItemTableManagerWidget::closeRequested);
    connect(Ramses::instance(), &Ramses::currentProjectChanged, this, &ItemTableManagerWidget::projectChanged);

}
