#include "itemtable.h"

ItemTable::ItemTable(QString title, QWidget *parent) : QWidget(parent)
{   
    setupUi(title);
    connectEvents();
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

    // Menus
    m_stepMenu = new QMenu(this);

    QAction *actionSelectAllSteps = new QAction("Select All", this);
    actionSelectAllSteps->setCheckable(true);
    m_stepMenu->addAction(actionSelectAllSteps);

    QAction *actionSelectNoSteps = new QAction("Select None", this);
    actionSelectNoSteps->setCheckable(true);
    m_stepMenu->addAction(actionSelectNoSteps);

    QAction *actionSelectMySteps = new QAction("Select my steps", this);
    actionSelectMySteps->setCheckable(true);
    m_stepMenu->addAction(actionSelectMySteps);

    m_stepMenu->addSeparator();

    QToolButton *stepButton = new QToolButton(this);
    stepButton->setText("Step");
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
    connect(m_titleBar, &TitleBar::closeRequested, this, &ItemTable::closeRequested);
    connect(Ramses::instance(), &Ramses::currentProjectChanged, this, &ItemTable::projectChanged);
}
