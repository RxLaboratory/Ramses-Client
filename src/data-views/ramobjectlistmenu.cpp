#include "ramobjectlistmenu.h"

RamObjectListMenu::RamObjectListMenu(bool checkable, QWidget *parent):
    QMenu(parent)
{
    m_emptyList = new RamObjectList(this);
    m_objects = new RamObjectFilterModel(this);
    m_objects->setSourceModel(m_emptyList);

    m_checkable = checkable;

    if (m_checkable)
    {
        QAction *all = new QAction("Select all");
        this->addAction(all);
        QAction *none = new QAction("Select none");
        this->addAction(none);
        this->addSeparator();

        connect(all,SIGNAL(triggered()),this,SLOT(selectAll()));
        connect(none,SIGNAL(triggered()),this,SLOT(selectNone()));
    }

    connect(m_objects, SIGNAL(rowsInserted(QModelIndex,int,int)),this,SLOT(newObject(QModelIndex,int,int)));
    connect(m_objects, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)),this,SLOT(removeObject(QModelIndex,int,int)));
    connect(m_objects, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),this,SLOT(objectChanged(QModelIndex,QModelIndex,QVector<int>)));
    connect(m_objects, SIGNAL(modelAboutToBeReset()),this,SLOT(clear()));
    connect(m_objects, SIGNAL(modelReset()),this,SLOT(reset()));
}

void RamObjectListMenu::setList(QAbstractItemModel *list)
{
    if (!list) m_objects->setList(m_emptyList);
    else m_objects->setList(list);
}

void RamObjectListMenu::addCreateButton()
{
    QAction *createAction = new QAction("Create new...");
    if (this->actions().count() > 0)
    {
        this->insertAction(this->actions().at(0), createAction);
        this->insertSeparator(this->actions().at(1));
    }
    else
    {
         this->addAction(createAction);
         this->addSeparator();
    }

    connect(createAction,SIGNAL(triggered()),this,SLOT(actionCreate()));
}

void RamObjectListMenu::setObjectVisible(RamObject *obj, bool visible)
{
    QList<QAction *> actions = this->actions();
    for(int j= actions.count() -1; j >= 0; j--)
    {
        quintptr iptr = actions.at(j)->data().toULongLong();
        if (iptr == 0) continue;
        RamObject *o = reinterpret_cast<RamObject*>( iptr );

        if (obj->is(o)) actions.at(j)->setVisible(visible);
    }
}

void RamObjectListMenu::showAll()
{
    QList<QAction *> actions = this->actions();
    for(int j= actions.count() -1; j >= 0; j--)
    {
        actions.at(j)->setVisible(true);
    }
}

void RamObjectListMenu::newObject(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent)

    for(int i = first; i <= last; i++)
    {
        quintptr iptr = m_objects->data( m_objects->index(i,0), Qt::UserRole).toULongLong();
        RamObject *o = reinterpret_cast<RamObject*>( iptr );
        QAction *a = new QAction( o->name() );
        a->setData( reinterpret_cast<quintptr>( o ) );
        a->setCheckable(m_checkable);
        if (m_checkable) a->setChecked(true);
        this->addAction(a);
        connect(a,SIGNAL(toggled(bool)),this,SLOT(actionAssign(bool)));
        connect(a,SIGNAL(triggered()),this,SLOT(actionAssign()));
    }
}

void RamObjectListMenu::removeObject(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent)

    for(int i = first; i <= last; i++)
    {
        quintptr iptr = m_objects->data( m_objects->index(i,0), Qt::UserRole).toULongLong();
        QList<QAction*> actions = this->actions();
        for (int j = actions.count() -1; j >= 0; j--)
        {
            if (actions.at(j)->data().toULongLong() == iptr)
            {
                actions.at(j)->deleteLater();
                break;
            }
        }
    }
}

void RamObjectListMenu::objectChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
    Q_UNUSED(roles)
    QList<QAction*> actions = this->actions();
    for (int i = topLeft.row(); i <= bottomRight.row(); i++)
    {
        quintptr iptr = m_objects->data( m_objects->index(i,0), Qt::UserRole).toULongLong();
        for (int j = actions.count() -1; j >= 0; j--)
        {
            if (actions.at(j)->data().toULongLong() == iptr)
            {
                RamObject *o = reinterpret_cast<RamObject*>( iptr );
                actions.at(j)->setText(o->name());
                break;
            }
        }
    }
}

void RamObjectListMenu::actionAssign(bool checked)
{
    QAction *a = qobject_cast<QAction*>( sender() );
    quintptr iptr = a->data().toULongLong();
    emit assign( reinterpret_cast<RamObject*>(iptr), checked );
}

void RamObjectListMenu::actionAssign()
{
    QAction *a = qobject_cast<QAction*>( sender() );
    quintptr iptr = a->data().toULongLong();
    emit assign( reinterpret_cast<RamObject*>(iptr) );
}

void RamObjectListMenu::actionCreate()
{
    emit create();
}

void RamObjectListMenu::clear()
{
    // Remove all
    QList<QAction*> actions = this->actions();
    for (int j = actions.count() -1; j >= 0; j--)
    {
        if (actions.at(j)->data().toULongLong() != 0)
            actions.at(j)->deleteLater();
    }
}

void RamObjectListMenu::reset()
{
    //Add all
    newObject(QModelIndex(),0,m_objects->rowCount()-1);
}

void RamObjectListMenu::selectAll()
{
    QList<QAction*> actions = this->actions();
    for (int j = actions.count() -1; j >= 0; j--)
    {
        if (actions.at(j)->data().toULongLong() != 0)
            actions.at(j)->setChecked(true);
    }
}

void RamObjectListMenu::selectNone()
{
    QList<QAction*> actions = this->actions();
    for (int j = actions.count() -1; j >= 0; j--)
    {
        if (actions.at(j)->data().toULongLong() != 0)
            actions.at(j)->setChecked(false);
    }
}

void RamObjectListMenu::filter(RamObject *o)
{
    //clear();
    if (o) m_objects->setFilterUuid( o->uuid() );
    else m_objects->setFilterUuid( "" );
    //reset();
}

void RamObjectListMenu::select(RamObject *o)
{
    if (!o) selectNone();
    QList<QAction*> actions = this->actions();
    for (int j = actions.count() -1; j >= 0; j--)
    {
        RamObject *obj = reinterpret_cast<RamObject*>( actions.at(j)->data().toULongLong() );
        if (!obj) continue;
        if (obj->is(o))
            actions.at(j)->setChecked(true);
    }
}



