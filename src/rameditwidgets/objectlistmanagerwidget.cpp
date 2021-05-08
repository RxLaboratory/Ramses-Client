#include "objectlistmanagerwidget.h"

ObjectListManagerWidget::ObjectListManagerWidget(ObjectEditWidget *editWidget, QString title, QWidget *parent) :
    QWidget(parent)
{
    m_editWidget = editWidget;
    setupUi();
    connectEvents();
    m_listEditWidget->setTitle(title);
    clear();
}

ObjectListManagerWidget::ObjectListManagerWidget(RamObjectList *objectList, ObjectEditWidget *editWidget, QString title, QWidget *parent) :
    QWidget(parent)
{
    m_editWidget = editWidget;
    setupUi();
    connectEvents();
    m_listEditWidget->setTitle(title);
    setList(objectList);
}

ObjectListManagerWidget::ObjectListManagerWidget(RamObjectUberList *objectList, ObjectEditWidget *editWidget, QString title, QWidget *parent) :
    QWidget(parent)
{
    m_editWidget = editWidget;
    setupUi();
    connectEvents();
    m_listEditWidget->setTitle(title);
    setList(objectList);
}

void ObjectListManagerWidget::setList(RamObjectList *objectList)
{
    while  (m_listConnection.count() > 0 ) disconnect( m_listConnection.takeLast() );
    m_listEditWidget->setList( objectList );
    if (!objectList) return;
    m_listConnection << connect(objectList, &RamObjectList::objectAdded, this, &ObjectListManagerWidget::editNewObject);
    this->setEnabled(true);
}

void ObjectListManagerWidget::setList(RamObjectUberList *objectList)
{
    while  (m_listConnection.count() > 0 ) disconnect( m_listConnection.takeLast() );
    m_listEditWidget->setList( objectList );
    if (!objectList) return;
    m_listConnection << connect(objectList, &RamObjectList::objectAdded, this, &ObjectListManagerWidget::editNewObject);
    this->setEnabled(true);
}

void ObjectListManagerWidget::clear()
{
    this->setEnabled(false);
    while  (m_listConnection.count() > 0 ) disconnect( m_listConnection.takeLast() );
    m_listEditWidget->clear();
}

void ObjectListManagerWidget::editNewObject(RamObject *o)
{
    if (o->shortName() == "NEW")
    {
        m_listEditWidget->select(o);
        m_editWidget->setObject(o);
    }
}

void ObjectListManagerWidget::setupUi()
{
    QHBoxLayout *lay = new QHBoxLayout(this);
    lay->setContentsMargins(3,0,0,0);

    QSplitter *splitter = new QSplitter(this);
    splitter->setOrientation(Qt::Horizontal);
    splitter->setHandleWidth(9);

    m_listEditWidget = new ObjectListEditWidget( false, this);
    splitter->addWidget(m_listEditWidget);

    QWidget *eWidget = new QWidget(this);
    QHBoxLayout *editLayout = new QHBoxLayout(eWidget);

    editLayout->addStretch();

    editLayout->addWidget(m_editWidget);

    editLayout->addStretch();

    editLayout->setStretch(0,20);
    editLayout->setStretch(1,80);
    editLayout->setStretch(2,20);

    splitter->addWidget(eWidget);

    lay->addWidget(splitter);

    splitter->setSizes(QList<int>() << 20 << 80);
}

void ObjectListManagerWidget::connectEvents()
{
    connect( m_listEditWidget, &ObjectListEditWidget::objectSelected, m_editWidget, &ObjectEditWidget::setObject );
    connect( m_listEditWidget, &ObjectListEditWidget::add, this, &ObjectListManagerWidget::createObject );
}

QString ObjectListManagerWidget::currentFilter() const
{
    return m_listEditWidget->currentFilter();
}
