#include "ramobjectlistmenu.h"

template<typename RO>
RamObjectListMenu<RO>::RamObjectListMenu(bool checkable, QWidget *parent):
    QMenu(parent)
{
    m_emptyList = new RamObjectList<RO>(this);
    m_objects = new RamObjectFilterModel<RO>(this);
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

template<typename RO>
void RamObjectListMenu<RO>::setList(RamObjectList<RO> *list)
{
    if (!list) m_objects->setList(m_emptyList);
    else m_objects->setList(list);
}

template<typename RO>
void RamObjectListMenu<RO>::addCreateButton()
{
    QAction *createAction = new QAction("Create new...");
    createAction->setData(-1);
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

template<typename RO>
RamObjectFilterModel<RO> *RamObjectListMenu<RO>::filteredList()
{
    return m_objects;
}

template<typename RO>
void RamObjectListMenu<RO>::setObjectVisible(RO obj, bool visible)
{
    QList<QAction *> actions = this->actions();
    for(int j= actions.count() -1; j >= 0; j--)
    {
        RO o = object(actions.at(j));
        if (obj->is(o)) actions.at(j)->setVisible(visible);
    }
}

template<typename RO>
void RamObjectListMenu<RO>::showAll()
{
    QList<QAction *> actions = this->actions();
    for(int j= actions.count() -1; j >= 0; j--)
    {
        actions.at(j)->setVisible(true);
    }
}

template<typename RO>
bool RamObjectListMenu<RO>::isAllChecked() const
{
    QList<QAction*> as = this->actions();

    for (int i = 0; i < as.count(); i++)
    {
        if (as.at(i)->isCheckable() && !as.at(i)->isChecked()) return false;
    }

    return true;
}

template<typename RO>
void RamObjectListMenu<RO>::newObject(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent)

    for(int i = first; i <= last; i++)
    {
        RO o = m_objects->at(i);
        QAction *a = new QAction( o->name() );
        a->setData( i );
        a->setCheckable(m_checkable);
        if (m_checkable) a->setChecked(true);
        this->addAction(a);
        connect(a,SIGNAL(toggled(bool)),this,SLOT(actionAssign(bool)));
        connect(a,SIGNAL(triggered()),this,SLOT(actionAssign()));
    }
}

template<typename RO>
void RamObjectListMenu<RO>::removeObject(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent)

    for(int i = first; i <= last; i++)
    {
        QList<QAction*> actions = this->actions();
        for (int j = actions.count() -1; j >= 0; j--)
        {
            if (actions.at(j)->data().toInt() == i)
            {
                actions.at(j)->deleteLater();
                break;
            }
        }
    }
}

template<typename RO>
void RamObjectListMenu<RO>::objectChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
    Q_UNUSED(roles)
    QList<QAction*> actions = this->actions();
    for (int i = topLeft.row(); i <= bottomRight.row(); i++)
    {
        for (int j = actions.count() -1; j >= 0; j--)
        {
            if (actions.at(j)->data().toInt() == i)
            {
                actions.at(j)->setText(m_objects->at(i)->name());
                break;
            }
        }
    }
}

template<typename RO>
void RamObjectListMenu<RO>::actionAssign(bool checked)
{
    QAction *a = qobject_cast<QAction*>( sender() );
    emit assign( object(a), checked );
}

template<typename RO>
void RamObjectListMenu<RO>::actionAssign()
{
    QAction *a = qobject_cast<QAction*>( sender() );
    emit assign( object(a) );
}

template<typename RO>
void RamObjectListMenu<RO>::actionCreate()
{
    emit createTriggered();
}

template<typename RO>
void RamObjectListMenu<RO>::clear()
{
    // Remove all
    QList<QAction*> actions = this->actions();
    for (int j = actions.count() -1; j >= 0; j--)
    {
        if (actions.at(j)->data().toInt() >= 0)
            actions.at(j)->deleteLater();
    }
}

template<typename RO>
void RamObjectListMenu<RO>::reset()
{
    //Add all
    newObject(QModelIndex(),0,m_objects->rowCount()-1);
}

template<typename RO>
QString RamObjectListMenu<RO>::objectUuid(QAction *a)
{
    RO obj = object(a);

    if (!obj) return a->text();

    return obj->uuid();
}

template<typename RO>
RO RamObjectListMenu<RO>::object(QAction *a)
{
    int oIndex = a->data().toInt();
    RO obj = m_objects->at(oIndex);
    return obj;
}

template<typename RO>
void RamObjectListMenu<RO>::selectAll()
{
    QList<QAction*> actions = this->actions();
    for (int j = actions.count() -1; j >= 0; j--)
    {
        QAction *a = actions.at(j);
        if (a->isCheckable()) a->setChecked(true);

        RO *o = object(a);
        if (o) emit assign( o, true );
    }
}

template<typename RO>
void RamObjectListMenu<RO>::selectNone()
{
    QList<QAction*> actions = this->actions();
    for (int j = actions.count() -1; j >= 0; j--)
    {
        QAction *a = actions.at(j);
        if (a->isCheckable()) a->setChecked(false);

        RO *o = object(a);
        if (o) emit assign( o, false );
    }
}

template<typename RO>
void RamObjectListMenu<RO>::filter(RO o)
{
    //clear();
    if (o) m_objects->setFilterUuid( o->uuid() );
    else m_objects->setFilterUuid( "" );
    //reset();
}

template<typename RO>
void RamObjectListMenu<RO>::select(RO o)
{
    if (!o) selectNone();
    QList<QAction*> actions = this->actions();
    for (int j = actions.count() -1; j >= 0; j--)
    {
        RO obj = object( actions.at(j) );
        if (o->is(obj))
            actions.at(j)->setChecked(true);
    }
}

template<typename RO>
void RamObjectListMenu<RO>::saveState(QSettings *settings, QString group) const
{
    settings->beginGroup(group);
    QList<QAction*> as = this->actions();
    for (int i = 0; i < as.count(); i++)
    {
        QAction *a = as.at(i);
        if (!a->isCheckable()) continue;

        QString uuid = objectUuid(a);

        settings->setValue(uuid, a->isChecked() );
    }
    settings->endGroup();
}

template<typename RO>
void RamObjectListMenu<RO>::restoreState(QSettings *settings, QString group)
{
    settings->beginGroup(group);

    QList<QAction*> as = this->actions();

    for (int i = 0; i < as.count(); i++)
    {
        QAction *a = as.at(i);

        RO obj = object(a);
        QString uuid = objectUuid(a);

        bool ok = settings->value(uuid, true).toBool();
        a->setChecked( ok );
        if (obj) emit assign( obj, ok );
    }

    settings->endGroup();
}



