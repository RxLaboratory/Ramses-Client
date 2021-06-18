#include "ramobjectlistmodel.h"

RamObjectListModel::RamObjectListModel(QObject *parent)
    : QAbstractListModel(parent)
{

}

int RamObjectListModel::rowCount(const QModelIndex & /*parent*/) const
{
   return m_objectsList.count();
}

int RamObjectListModel::columnCount(const QModelIndex & /*parent*/) const
{
    return 1;
}

QVariant RamObjectListModel::data(const QModelIndex &index, int role) const
{
    Q_UNUSED(role);

    // Pass the pointer as an int to our delegate
    RamObject *obj = m_objectsList.at(index.row());
    quintptr iptr = reinterpret_cast<quintptr>(obj);
    return iptr;
}

void RamObjectListModel::objectChanged(RamObject *obj)
{
    // Get row
    int row = objRow(obj);
    if (row<0) return;

    QModelIndex index = createIndex(row,0);
    emit dataChanged(index, index, {Qt::DisplayRole});
}

RamObject *RamObjectListModel::fromUuid(QString uuid) const
{
    return m_objects.value(uuid, nullptr);
}

RamObject *RamObjectListModel::fromName(QString shortName, QString name) const
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

void RamObjectListModel::connectObject(RamObject *obj)
{
    QList<QMetaObject::Connection> c;
    c << connect( obj, SIGNAL(removed(RamObject*)), this, SLOT(removeAll(RamObject*)));
    //c << connect( obj, &RamObject::orderChanged, this, &RamObjectList::sort);
    c << connect( obj, &RamObject::orderChanged, this, &RamObjectListModel::objectChanged);
    m_connections[obj->uuid()] = c;
}

int RamObjectListModel::objRow(RamObject *obj)
{
    for (int i = m_objectsList.count() - 1; i >= 0; i--)
    {
        if ( m_objectsList.at(i)->is(obj) ) return i;
    }
    return -1;
}

void RamObjectListModel::append(RamObject *obj)
{
    if (contains(obj)) return;

    beginInsertRows(QModelIndex(), m_objectsList.count(), m_objectsList.count());

    m_objects[ obj->uuid() ] = obj;
    m_objectsList << obj;
    connectObject(obj);

    endInsertRows();
}

void RamObjectListModel::insert(int i, RamObject *obj)
{
    if (contains(obj)) return;

    beginInsertRows(QModelIndex(), i, i);

    m_objectsList.insert(i , obj);
    m_objects[obj->uuid()] = obj;
    connectObject(obj);

    endInsertRows();
}

void RamObjectListModel::clear()
{
    beginResetModel();

    m_objects.clear();
    m_objectsList.clear();

    // Disconnect all
    QMapIterator<QString, QList<QMetaObject::Connection>> i(m_connections);
    while (i.hasNext()) {
        i.next();
        QList<QMetaObject::Connection> c = i.value();
        while(!c.isEmpty()) disconnect(c.takeLast());
    }

    endResetModel();
}

RamObject *RamObjectListModel::at(int i) const
{
    return m_objectsList.at(i);
}

RamObject *RamObjectListModel::last() const
{
    if (m_objectsList.count() == 0) return nullptr;
    return m_objectsList.last();
}

void RamObjectListModel::removeAt(int i)
{
    takeAt(i);
}

void RamObjectListModel::removeFirst()
{
    if (m_objects.count() > 0) removeAt(0);
}

void RamObjectListModel::removeLast()
{
    if (m_objects.count() > 0) removeAt(m_objects.count() - 1);
}

RamObject *RamObjectListModel::takeAt(int i)
{
    beginRemoveRows(QModelIndex(), i, i);

    // take from list
    RamObject *obj = m_objectsList.takeAt(i);

    // remove from map
    if( m_objects.contains(obj->uuid()) )m_objects.remove(obj->uuid());

    // disconnect
    if (m_connections.contains(obj->uuid()))
    {
        QList<QMetaObject::Connection> c = m_connections.take(obj->uuid());
        while (!c.isEmpty()) disconnect( c.takeLast() );
    }

    endRemoveRows();
    return obj;
}

RamObject *RamObjectListModel::takeFromUuid(QString uuid)
{
    // get from map
    RamObject *obj = m_objects.value(uuid, nullptr);
    if (!obj) return nullptr;

    // get index from list to remove
    int row = objRow(obj);
    if (row<0) return nullptr;

    return takeAt(row);

    return nullptr;
}

int RamObjectListModel::count() const
{
    return m_objects.count();
}

bool RamObjectListModel::contains(RamObject *obj) const
{
    return contains(obj->uuid());
}

bool RamObjectListModel::contains(QString uuid) const
{
    return m_objects.contains( uuid );
}

void RamObjectListModel::removeAll(RamObject *obj)
{
    int row = objRow(obj);
    if (row<0) return;

    takeAt(row);
}

void RamObjectListModel::removeAll(QString uuid)
{
    // get from map
    RamObject *obj = m_objects.value(uuid, nullptr);
    if (!obj) return;

    int row = objRow(obj);
    if (row<0) return;

    takeAt(row);
}

bool objectSorter(RamObject *a, RamObject *b)
{
    if (a->order() != b->order()) return a->order() < b->order();
    else return a->shortName() < b->shortName();
}

/*void RamObjectListModel::sort()
{
    if (m_sorted) return;
    std::sort(m_objectsList.begin(), m_objectsList.end(), objectSorter);
    m_sorted = true;
}*/
