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
    m_objects = other.toList();
}

RamObjectList::~RamObjectList()
{
    emit cleared();
}

void RamObjectList::append(RamObject *obj)
{
    m_objects << obj;
    addObject(obj, m_objects.count() -1);
}

void RamObjectList::insert(int i, RamObject *obj)
{
    m_objects.insert(i , obj);
    addObject(obj, i);
}

void RamObjectList::clear()
{
    m_objects.clear();
    emit cleared();
}

RamObject *RamObjectList::fromUuid(QString uuid) const
{
    for (int i =0; i < m_objects.count(); i++)
    {
        if (m_objects.at(i)->uuid() == uuid) return m_objects.at(i);
    }
    return nullptr;
}

RamObject *RamObjectList::at(int i) const
{
    return m_objects.at(i);
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
    RamObject *obj = m_objects.takeAt(i);

    if (m_connections.contains(obj->uuid()))
    {
        QList<QMetaObject::Connection> c = m_connections.take(obj->uuid());
        while (!c.isEmpty()) disconnect( c.takeLast() );
    }

    emit objectRemoved(obj);

    return obj;
}

void RamObjectList::removeAll(RamObject *obj)
{
    for (int i = m_objects.count() -1; i >= 0; i--)
    {
        if ( m_objects.at(i)->is(obj) )
        {
            removeAt(i);
        }
    }
}

void RamObjectList::removeAll(QString uuid)
{
    for (int i = m_objects.count() -1; i >= 0; i--)
    {
        if ( m_objects.at(i)->uuid() == uuid )
        {
            removeAt(i);
        }
    }
}

QList<RamObject *> RamObjectList::toList() const
{
    return m_objects;
}

int RamObjectList::count() const
{
    return m_objects.count();
}

bool RamObjectList::contains(RamObject *obj) const
{
    for (int i =0; i < m_objects.count(); i++)
    {
        if (m_objects.at(i)->is(obj)) return true;
    }
    return false;
}

RamObject *RamObjectList::operator[](int i) const
{
    return m_objects[i];
}

bool objectSorter(RamObject *a, RamObject *b)
{
    if (a->order() != b->order()) return a->order() < b->order();
    else return a->shortName() < b->shortName();
}

void RamObjectList::sort()
{
    std::sort(m_objects.begin(), m_objects.end(), objectSorter);
}

void RamObjectList::addObject(RamObject *obj, int index)
{
    QList<QMetaObject::Connection> c;
    c << connect( obj, SIGNAL(removed(RamObject*)), this, SLOT(removeAll(RamObject*)));
    c << connect( obj, &RamObject::orderChanged, this, &RamObjectList::sort);
    m_connections[obj->uuid()] = c;
    emit objectAdded(obj, index);
}
