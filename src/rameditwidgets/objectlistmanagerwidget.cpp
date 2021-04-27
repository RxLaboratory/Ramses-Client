#include "objectlistmanagerwidget.h"

ObjectListManagerWidget::ObjectListManagerWidget(RamObjectList *objectList, ObjectEditWidget *editWidget, QWidget *parent) : QWidget(parent)
{
    m_editWidget = editWidget;
    m_objectList = objectList;
    setupUi();
    connectEvents();
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

    m_listEditWidget = new ObjectListEditWidget( m_objectList, false, this);
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
    connect(m_objectList, &RamObjectList::objectAdded, this, &ObjectListManagerWidget::editNewObject);
}

RamObjectList *ObjectListManagerWidget::objectList() const
{
    return m_objectList;
}

