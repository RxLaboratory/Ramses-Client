#include "itemtable.h"

ItemTable::ItemTable(QString title, QWidget *parent) : QWidget(parent)
{   
    setupUi(title);
    connectEvents();
}

void ItemTable::selectAllSteps()
{
    QList<QAction*> actions = m_stepMenu->actions();
    for (int i = 4; i < actions.count(); i++)
    {
        actions[i]->setChecked(true);
    }
}

void ItemTable::selectUserSteps()
{
    QList<QAction*> actions = m_stepMenu->actions();
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

void ItemTable::deselectSteps()
{
    QList<QAction*> actions = m_stepMenu->actions();
    for (int i = 4; i < actions.count(); i++)
    {
        actions[i]->setChecked(false);
    }
}

void ItemTable::showEvent(QShowEvent *event)
{
    if (!event->spontaneous()) m_titleBar->show();
    QWidget::showEvent(event);
}

void ItemTable::hideEvent(QHideEvent *event)
{
    if (!event->spontaneous())
    {
        m_titleBar->hide();
    }
    QWidget::hideEvent(event);
}

void ItemTable::stepAdded(RamStep *step)
{
    QAction *stepAction = new QAction(step->name(), this);
    stepAction->setCheckable(true);
    stepAction->setData(step->uuid());
    stepAction->setChecked(true);
    m_stepMenu->addAction(stepAction);
    connect (stepAction, SIGNAL(toggled(bool)), this, SLOT(stepActionToggled(bool)));
}

void ItemTable::stepRemoved(RamObject *stepObj)
{
    QList<QAction*> actions = m_stepMenu->actions();
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

void ItemTable::stepActionToggled(bool checked)
{
    QAction *action = (QAction*)sender();
    //m_table->setStepVisible( action->data().toString(), checked);
}

void ItemTable::setupUi(QString title)
{
    // Get the mainwindow to add the titlebar
    QMainWindow *mw = GuiUtils::appMainWindow();
    mw->addToolBarBreak(Qt::TopToolBarArea);

    m_titleBar = new TitleBar(title,false, mw);
    m_titleBar->showReinitButton(false);
    mw->addToolBar(Qt::TopToolBarArea,m_titleBar);
    m_titleBar->setFloatable(false);
    m_titleBar->hide();

    // Search field
    m_searchEdit = new DuQFSearchEdit(this);
    m_searchEdit->setMaximumWidth(150);
    m_titleBar->insertLeft(m_searchEdit);

    // Menus
    m_stepMenu = new QMenu(this);

    m_actionSelectAllSteps = new QAction("Select All", this);
    m_stepMenu->addAction(m_actionSelectAllSteps);

    m_actionSelectNoSteps = new QAction("Select None", this);
    m_stepMenu->addAction(m_actionSelectNoSteps);

    m_actionSelectMySteps = new QAction("Select my steps", this);
    m_stepMenu->addAction(m_actionSelectMySteps);

    m_stepMenu->addSeparator();

    QToolButton *stepButton = new QToolButton(this);
    stepButton->setText("Steps");
    stepButton->setIcon(QIcon(":/icons/step"));
    stepButton->setIconSize(QSize(16,16));
    stepButton->setObjectName("menuButton");
    stepButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    stepButton->setPopupMode(QToolButton::InstantPopup);
    stepButton->setMenu(m_stepMenu);

    m_titleBar->insertLeft(stepButton);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->setSpacing(3);
    mainLayout->setContentsMargins(0,0,0,0);

    m_table = new ItemTableWidget(this);
    mainLayout->addWidget(m_table);

    this->setLayout(mainLayout);
}

void ItemTable::connectEvents()
{
    connect(m_actionSelectAllSteps, SIGNAL(triggered()), this, SLOT(selectAllSteps()));
    connect(m_actionSelectNoSteps, SIGNAL(triggered()), this, SLOT(deselectSteps()));
    connect(m_actionSelectMySteps, SIGNAL(triggered()), this, SLOT(selectUserSteps()));
    /*connect(m_searchEdit, &DuQFSearchEdit::changing, m_table, &ItemTableWidget::search);
    connect(m_searchEdit, &DuQFSearchEdit::changed, m_table, &ItemTableWidget::search);
    connect(m_table, &ItemTableWidget::newStep, this, &ItemTable::stepAdded);
    connect(m_table, &ItemTableWidget::stepRemoved, this, &ItemTable::stepRemoved);*/
    connect(m_titleBar, &TitleBar::closeRequested, this, &ItemTable::closeRequested);
    connect(Ramses::instance(), &Ramses::currentProjectChanged, this, &ItemTable::projectChanged);

}
