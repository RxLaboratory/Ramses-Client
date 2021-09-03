#include "objectlistmanagerwidget.h"

ObjectListManagerWidget::ObjectListManagerWidget(ObjectEditWidget *editWidget, QString title, QIcon icon, QWidget *parent) :
    QWidget(parent)
{
    m_editWidget = editWidget;
    setupUi(title,icon);
    connectEvents();
    m_listEditWidget->setTitle(title);
    clear();
}

ObjectListManagerWidget::ObjectListManagerWidget(RamObjectList *objectList, ObjectEditWidget *editWidget, QString title, QIcon icon, QWidget *parent) :
    QWidget(parent)
{
    m_editWidget = editWidget;
    setupUi(title,icon);
    connectEvents();
    m_listEditWidget->setTitle(title);
    setList(objectList);
}

void ObjectListManagerWidget::setList(RamObjectList *objectList)
{
    while  (m_listConnection.count() > 0 ) disconnect( m_listConnection.takeLast() );
    m_listEditWidget->setList( objectList );
    if (!objectList) return;
    this->setEnabled(true);
}

void ObjectListManagerWidget::clear()
{
    this->setEnabled(false);
    while  (m_listConnection.count() > 0 ) disconnect( m_listConnection.takeLast() );
    m_listEditWidget->clear();
}

void ObjectListManagerWidget::setupUi(QString title, QIcon icon)
{
    QHBoxLayout *lay = new QHBoxLayout(this);
    lay->setContentsMargins(3,0,0,0);

    m_splitter = new QSplitter(this);
    m_splitter->setOrientation(Qt::Horizontal);
    m_splitter->setHandleWidth(9);

    m_listEditWidget = new ObjectListEditWidget( false, RamUser::Admin, this);
    m_splitter->addWidget(m_listEditWidget);

    QWidget *eWidget = new QWidget(this);
    QHBoxLayout *editLayout = new QHBoxLayout(eWidget);

    editLayout->addStretch();

    editLayout->addWidget(m_editWidget);

    editLayout->addStretch();

    editLayout->setStretch(0,20);
    editLayout->setStretch(1,80);
    editLayout->setStretch(2,20);

    m_splitter->addWidget(eWidget);

    lay->addWidget(m_splitter);

    // Menu
    QMenu *itemMenu = new QMenu();

    ui_createAction = new QAction(QIcon(":icons/add"), "Create new", this);
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

void ObjectListManagerWidget::connectEvents()
{
    connect( m_listEditWidget, SIGNAL(objectSelected(RamObject*)), m_editWidget, SLOT(setObject(RamObject*)) );
    connect( m_listEditWidget, SIGNAL(add()), this, SLOT(createObject()) );
    connect( ui_createAction, SIGNAL(triggered()), this, SLOT(createEditObject()) );
    connect( ui_createShortcut, SIGNAL(activated()), this, SLOT(createObject()));
    connect( ui_removeShortcut, SIGNAL(activated()), m_listEditWidget, SLOT(removeSelectedObjects()));
}

QString ObjectListManagerWidget::currentFilter() const
{
    return m_listEditWidget->currentFilterUuid();
}

void ObjectListManagerWidget::editObject(RamObject *o)
{
    m_listEditWidget->select(o);
    m_editWidget->setObject(o);
}

QToolButton *ObjectListManagerWidget::menuButton()
{
    return ui_itemButton;
}

void ObjectListManagerWidget::showEvent(QShowEvent *event)
{
    m_splitter->setSizes(QList<int>() << 400 << 1000);
    QWidget::showEvent(event);
}

void ObjectListManagerWidget::createEditObject()
{
    RamObject *o = createObject();
    if (o) o->edit();
}
