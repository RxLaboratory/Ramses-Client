#include "ramobjectuberlist.h"

RamObjectUberList::RamObjectUberList(QObject *parent):
    RamObjectList(parent)
{

}

RamObject *RamObjectUberList::objectFromUuid(QString uuid)
{
    for (int i = 0 ; i < m_objects.count(); i++)
    {
        RamObjectList *list = (RamObjectList*)m_objects.at(i);
        for (int j = 0; j < list->count(); j++)
        {
            if ( list->at(i)->uuid() == uuid ) return list->at(i);
        }
    }
    return nullptr;
}

int RamObjectUberList::objectCount()
{
    int c = 0;
    for (int i = 0 ; i < m_objects.count(); i++)
    {
        RamObjectList *list = (RamObjectList*)m_objects.at(i);
        c += list->count();
    }
    return c;
}

bool RamObjectUberList::containsObject(RamObject *obj) const
{
    for (int i = 0 ; i < m_objects.count(); i++)
    {
        RamObjectList *list = (RamObjectList*)m_objects.at(i);
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
        RamObjectList *list = (RamObjectList*)m_objects.at(i);
        c += list->count();
        if (index > c) continue;
        return list->at(index);
    }
    return nullptr;
}

void RamObjectUberList::removeObject(QString uuid)
{
    for (int i = 0; i < m_objects.count(); i++)
    {
        RamObjectList *list = (RamObjectList*)m_objects.at(i);
        list->removeAll(uuid);
    }
}

void RamObjectUberList::removeObject(RamObject *obj)
{
    for (int i = 0; i < m_objects.count(); i++)
    {
        RamObjectList *list = (RamObjectList*)m_objects.at(i);
        list->removeAll(obj);
    }
}

RamObject *RamObjectUberList::takeAt(int i)
{
    QSignalBlocker b(this);
    RamObject *obj = m_objects.at(i);
    if (m_connections.contains(obj->uuid()))
    {
        QList<QMetaObject::Connection> c = m_connections.take(obj->uuid());
        while (!c.isEmpty()) disconnect( c.takeLast() );
    }
    return RamObjectList::takeAt(i);
}

void RamObjectUberList::addObject(RamObject *obj, int index)
{
    QSignalBlocker b(this);
    RamObjectList::addObject(obj, index);
    QList<QMetaObject::Connection> c;
    RamObjectList *list = (RamObjectList*)obj;
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

