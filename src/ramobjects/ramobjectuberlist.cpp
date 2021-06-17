#include "ramobjectuberlist.h"

RamObjectUberList::RamObjectUberList(QString name, QObject *parent):
    RamObjectList("", name, "", parent)
{
    this->setObjectName( "RamObjectUberList " + name );
    this->setObjectType(ObjectUberList);
}

RamObjectUberList::RamObjectUberList(QObject *parent):
    RamObjectList(parent)
{
    this->setObjectName( "RamObjectUberList" );
    this->setObjectType(ObjectUberList);
}

RamObject *RamObjectUberList::objectFromUuid(QString uuid)
{
    for (int i = 0 ; i < m_objects.count(); i++)
    {
        RamObjectList *list = qobject_cast<RamObjectList*>( m_objectsList.at(i) );
        RamObject *o = list->fromUuid(uuid);
        if (o) return o;
    }
    return nullptr;
}

RamObject *RamObjectUberList::objectFromName(QString shortName, QString name)
{
    for (int i = 0 ; i < m_objects.count(); i++)
    {
        RamObjectList *list = qobject_cast<RamObjectList*>( m_objectsList.at(i) );
        RamObject *o = list->fromName(shortName, name);
        if (o) return o;
    }
    return nullptr;
}

int RamObjectUberList::objectCount()
{
    int c = 0;
    for (int i = 0 ; i < m_objects.count(); i++)
    {
        RamObjectList *list = qobject_cast<RamObjectList*>( m_objectsList.at(i) );
        c += list->count();
    }
    return c;
}

bool RamObjectUberList::containsObject(RamObject *obj) const
{
    for (int i = 0 ; i < m_objects.count(); i++)
    {
        RamObjectList *list = qobject_cast<RamObjectList*>( m_objectsList.at(i) );
        for (int j = 0; j < list->count(); j++)
        {
            if ( list->at(i)->is(obj) ) return true;
        }
    }
    return false;
}

RamObject *RamObjectUberList::objectAt(int index) const
{
    int c = 0;
    for (int i = 0 ; i < m_objects.count(); i++)
    {
        RamObjectList *list = qobject_cast<RamObjectList*>( m_objectsList.at(i) );
        int previousCount = c;
        c += list->count();
        if (index >= c) continue;
        return list->at(index-previousCount);
    }
    return nullptr;
}

void RamObjectUberList::removeObject(QString uuid)
{
    for (int i = 0; i < m_objects.count(); i++)
    {
        RamObjectList *list = qobject_cast<RamObjectList*>( m_objectsList.at(i) );
        list->removeAll(uuid);
    }
}

void RamObjectUberList::removeObject(RamObject *obj)
{
    for (int i = 0; i < m_objects.count(); i++)
    {
        RamObjectList *list = qobject_cast<RamObjectList*>( m_objectsList.at(i) );
        list->removeAll(obj);
    }
}

RamObject *RamObjectUberList::takeObject(QString uuid)
{
    for (int i = 0; i < m_objects.count(); i++)
    {
        RamObjectList *list = qobject_cast<RamObjectList*>( m_objectsList.at(i) );
        RamObject *obj = list->takeFromUuid(uuid);
        if (obj) return obj;
    }
    return nullptr;
}

RamObject *RamObjectUberList::takeAt(int i)
{
    RamObject *obj = m_objectsList.at(i);
    if (m_connections.contains(obj->uuid()))
    {
        QList<QMetaObject::Connection> c = m_connections.take(obj->uuid());
        while (!c.isEmpty()) disconnect( c.takeLast() );
    }
    return RamObjectList::takeAt(i);
}

void RamObjectUberList::addObject(RamObject *obj, int index)
{
    RamObjectList::addObject(obj, index);
    QList<QMetaObject::Connection> c;
    RamObjectList *list = qobject_cast<RamObjectList*>( obj );
    if (!list) return;
    c << connect(list, &RamObjectList::objectAdded, this, &RamObjectUberList::relayObjectAdded);
    c << connect(list, &RamObjectList::objectRemoved, this, &RamObjectUberList::relayObjectRemoved);
}

void RamObjectUberList::relayObjectAdded(RamObject *o, int index)
{
    emit objectAdded(o, index);
}

void RamObjectUberList::relayObjectRemoved(RamObject *o)
{
    emit objectRemoved(o);
}

