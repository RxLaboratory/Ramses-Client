#include "ramobjectlist.h"

RamObjectList::RamObjectList(QObject *parent) :
    RamObject(parent)
{

}

RamObjectList::RamObjectList(QString shortName, QString name, QString uuid, QObject *parent) :
    RamObject(shortName, name, uuid, parent)
{

}

RamObjectList::RamObjectList(const RamObjectList &other) :
    RamObject(other.parent())
{
    m_objectsList = other.toList();
    m_objects = other.toMap();
}

RamObjectList::~RamObjectList()
{
    emit cleared();
}

void RamObjectList::append(RamObject *obj)
{
    if (contains(obj)) return;
    m_objects[obj->uuid()] = obj;
    m_objectsList << obj;
    addObject(obj, m_objects.count() -1);
}

void RamObjectList::insert(int i, RamObject *obj)
{
    if (contains(obj)) return;
    m_objectsList.insert(i , obj);
    m_objects[obj->uuid()] = obj;
    addObject(obj, i);
}

void RamObjectList::clear()
{
    m_objects.clear();
    m_objectsList.clear();
    emit cleared();
}

RamObject *RamObjectList::fromUuid(QString uuid) const
{
    return m_objects.value(uuid, nullptr);
}

RamObject *RamObjectList::fromName(QString shortName, QString name) const
{
    QMapIterator<QString, RamObject*> i(m_objects);
    while (i.hasNext()) {
        i.next();
        RamObject *o = i.value();
        if (o->shortName() == shortName)
        {
            if (name == "") return o;
            if (o->name() == name) return o;
        }
    }
    return nullptr;
}

RamObject *RamObjectList::at(int i) const
{
    return m_objectsList.at(i);
}

RamObject *RamObjectList::last() const
{
    return m_objectsList.last();
}

void RamObjectList::removeAt(int i)
{
    takeAt(i);
}

void RamObjectList::removeFirst()
{
    if (m_objects.count() > 0) removeAt(0);
}

void RamObjectList::removeLast()
{
    if (m_objects.count() > 0) removeAt(m_objects.count() - 1);
}

RamObject *RamObjectList::takeAt(int i)
{
    // take from list
    RamObject *obj = m_objectsList.takeAt(i);

    // remove from map
    m_objects.remove(obj->uuid());

    // disconnect
    if (m_connections.contains(obj->uuid()))
    {
        QList<QMetaObject::Connection> c = m_connections.take(obj->uuid());
        while (!c.isEmpty()) disconnect( c.takeLast() );
    }

    emit objectRemoved(obj);

    return obj;
}

RamObject *RamObjectList::takeFromUuid(QString uuid)
{
    // get from map
    RamObject *obj = m_objects.value(uuid, nullptr);
    if (!obj) return nullptr;

    // get index from list to remove
    for (int i = m_objectsList.count() - 1; i >= 0; i--)
    {
        if ( m_objectsList.at(i)->is(obj) ) return takeAt(i);
    }

    return nullptr;
}

void RamObjectList::removeAll(RamObject *obj)
{
    // get index from list to remove
    for (int i = m_objectsList.count() - 1; i >= 0; i--)
    {
        if ( m_objectsList.at(i)->is(obj) ) takeAt(i);
    }
}

void RamObjectList::removeAll(QString uuid)
{
    // get from map
    RamObject *obj = m_objects.value(uuid, nullptr);
    if (!obj) return;

    // get index from list to remove
    for (int i = m_objectsList.count() - 1; i >= 0; i--)
    {
        if ( m_objectsList.at(i)->is(obj) ) takeAt(i);
    }
}

QList<RamObject *> RamObjectList::toList() const
{
    return m_objectsList;
}

QMap<QString, RamObject *> RamObjectList::toMap() const
{
    return m_objects;
}

int RamObjectList::count() const
{
    return m_objects.count();
}

bool RamObjectList::contains(RamObject *obj) const
{
    return m_objects.contains(obj->uuid());
}

RamObject *RamObjectList::operator[](int i) const
{
    return m_objectsList[i];
}

bool objectSorter(RamObject *a, RamObject *b)
{
    if (a->order() != b->order()) return a->order() < b->order();
    else return a->shortName() < b->shortName();
}

void RamObjectList::sort()
{
    std::sort(m_objectsList.begin(), m_objectsList.end(), objectSorter);
}

void RamObjectList::addObject(RamObject *obj, int index)
{
    QList<QMetaObject::Connection> c;
    c << connect( obj, SIGNAL(removed(RamObject*)), this, SLOT(removeAll(RamObject*)));
    c << connect( obj, &RamObject::orderChanged, this, &RamObjectList::sort);
    m_connections[obj->uuid()] = c;
    emit objectAdded(obj, index);
}
