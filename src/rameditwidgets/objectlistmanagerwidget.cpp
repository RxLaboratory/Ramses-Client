#include "objectlistmanagerwidget.h"


template<typename RO, typename ROF>
ObjectListManagerWidget<RO,ROF>::ObjectListManagerWidget(QString title, QIcon icon, QWidget *parent) :
    QWidget(parent)
{
    setupUi(title,icon);
    connectEvents();
    m_listEditWidget->setTitle(title);
    clear();
}

template<typename RO, typename ROF>
ObjectListManagerWidget<RO,ROF>::ObjectListManagerWidget(RamObjectList<RO> *objectList, QString title, QIcon icon, QWidget *parent) :
    QWidget(parent)
{
    setupUi(title,icon);
    connectEvents();
    m_listEditWidget->setTitle(title);
    setList(objectList);
}

template<typename RO, typename ROF>
void ObjectListManagerWidget<RO,ROF>::setList(RamObjectList<RO> *objectList)
{
    m_listEditWidget->setList( objectList );
    if (!objectList) return;
    this->setEnabled(true);
}

template<typename RO, typename ROF>
void ObjectListManagerWidget<RO,ROF>::clear()
{
    this->setEnabled(false);
    m_listEditWidget->clear();
}

template<typename RO, typename ROF>
void ObjectListManagerWidget<RO,ROF>::setupUi(QString title, QIcon icon)
{
    QHBoxLayout *lay = new QHBoxLayout(this);
    lay->setContentsMargins(3,0,0,0);

    m_listEditWidget = new ObjectListEditWidget<RO,ROF>( false, RamUser::Admin, this);
    m_listEditWidget->setMaximumWidth(500);
    lay->addWidget(m_listEditWidget);

    lay->addStretch();

    // Menu
    QMenu *itemMenu = new QMenu();

    ui_createAction = new QAction("Create new", this);
    itemMenu->addAction(ui_createAction);

    ui_itemButton = new QToolButton();
    ui_itemButton->setIcon(icon);
    ui_itemButton->setText(" " + title);
    ui_itemButton->setMenu(itemMenu);
    ui_itemButton->setIconSize(QSize(16,16));
    ui_itemButton->setObjectName("menuButton");
    ui_itemButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    ui_itemButton->setPopupMode(QToolButton::InstantPopup);

    // Shortcuts
    ui_createShortcut = new QShortcut(QKeySequence("Shift+A"),this);
    ui_removeShortcut = new QShortcut(QKeySequence("Shift+X"),this);
}

template<typename RO, typename ROF>
void ObjectListManagerWidget<RO,ROF>::connectEvents()
{
    connect( m_listEditWidget, SIGNAL(add()), this, SLOT(createObject()) );
    connect( ui_createAction, SIGNAL(triggered()), this, SLOT(createEditObject()) );
    connect( ui_createShortcut, SIGNAL(activated()), this, SLOT(createObject()));
    connect( ui_removeShortcut, SIGNAL(activated()), m_listEditWidget, SLOT(removeSelectedObjects()));
}

template<typename RO, typename ROF>
QString ObjectListManagerWidget<RO,ROF>::currentFilter() const
{
    return m_listEditWidget->currentFilterUuid();
}

template<typename RO, typename ROF>
QToolButton *ObjectListManagerWidget<RO,ROF>::menuButton()
{
    return ui_itemButton;
}

template<typename RO, typename ROF>
void ObjectListManagerWidget<RO,ROF>::createEditObject()
{
    RamObject *o = createObject();
    if (o) o->edit();
}
