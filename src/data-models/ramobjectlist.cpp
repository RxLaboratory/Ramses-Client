#include "ramobjectlist.h"

#include "ramstatus.h"
#include "ramstate.h"

RamObjectList::RamObjectList(QObject *parent)
    : QAbstractTableModel(parent)
{
    this->setObjectName( "RamObjectList" );
}

RamObjectList::RamObjectList(QString shortName, QString name, QObject *parent)
    : QAbstractTableModel(parent)
{
    m_shortName = shortName;
    m_name = name;

    this->setObjectName( "RamObjectList" );
}

int RamObjectList::rowCount(const QModelIndex & parent) const
{
    Q_UNUSED(parent);
    return m_objectsList.count();
}

int RamObjectList::columnCount(const QModelIndex & parent) const
{
    Q_UNUSED(parent);
    return 1;
}

QVariant RamObjectList::data(const QModelIndex &index, int role) const
{
    Q_UNUSED(role);

    // Pass the pointer as an int to our delegate
    RamObject *obj = m_objectsList.at(index.row());

    if (role == Qt::DisplayRole) return obj->name();

    if (role == Qt::StatusTipRole)
        return QString(obj->shortName() % " | " % obj->name());

    if (role == Qt::ToolTipRole) return QString(obj->shortName() % " | " % obj->name() % "\n" % obj->comment());

    if (role == Qt::InitialSortOrderRole) return obj->order();

    quintptr iptr = reinterpret_cast<quintptr>(obj);
    return iptr;
}

QVariant RamObjectList::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Vertical)
    {
        if ( role == Qt::DisplayRole )
            return m_objectsList.at( section )->shortName();
        if ( role == Qt::UserRole )
            return m_objectsList.at( section )->uuid();
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}

void RamObjectList::sort(int column, Qt::SortOrder order)
{
    Q_UNUSED(column)
    Q_UNUSED(order)

    if (m_sorted) return;
    std::sort(m_objectsList.begin(), m_objectsList.end(), objectSorter);

    m_sorted = true;
}

void RamObjectList::objectChanged(RamObject *obj)
{
    // Get row
    int row = objRow(obj);
    if (row < 0) return;
    if (row >= rowCount()) return;

    QModelIndex index = createIndex(row,0);
    emit dataChanged(index, index, {});
    emit headerDataChanged(Qt::Vertical, row, row);
    emit objectDataChanged(obj);
}

void RamObjectList::objectInserted(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent)

    for (int i = first; i <= last; i++)
    {
        emit objectInserted( m_objectsList.at(i) );
    }
}

void RamObjectList::objectRemoved(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent)

    for (int i = first; i <= last; i++)
    {
        emit objectRemoved( m_objectsList.at(i) );
    }
}

const QString &RamObjectList::name() const
{
    return m_name;
}

void RamObjectList::setName(const QString &newName)
{
    m_name = newName;
}

const QString &RamObjectList::shortName() const
{
    return m_shortName;
}

void RamObjectList::setShortName(const QString &newShortName)
{
    m_shortName = newShortName;
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

void RamObjectList::connectObject(RamObject *obj)
{
    QList<QMetaObject::Connection> c;
    c << connect( obj, SIGNAL(removed(RamObject*)), this, SLOT(removeAll(RamObject*)));
    c << connect( obj, SIGNAL(changed(RamObject*)), this, SLOT(objectChanged(RamObject*)));
    m_connections[obj->uuid()] = c;
}

int RamObjectList::objRow(RamObject *obj)
{
    for (int i = m_objectsList.count() - 1; i >= 0; i--)
    {
        if ( m_objectsList.at(i)->is(obj) ) return i;
    }
    return -1;
}

void RamObjectList::append(RamObject *obj)
{

    if (!obj) return;
    if (contains(obj)) return;

    insertObject(this->m_objectsList.count(), obj);
}

void RamObjectList::insertObject(int i, RamObject *obj)
{
    if (contains(obj)) return;

    beginInsertRows(QModelIndex(), i, i);

    m_objectsList.insert(i , obj);
    m_objects[obj->uuid()] = obj;
    connectObject(obj);

    if (obj->order() == -1)
    {
        QSignalBlocker b(obj);
        obj->setOrder(i);
        obj->update();
    }

    m_sorted = false;
    endInsertRows();
}

void RamObjectList::clear()
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

    m_sorted = true;
    endResetModel();
}

RamObject *RamObjectList::at(int i) const
{
    return m_objectsList.at(i);
}

RamObject *RamObjectList::last() const
{
    if (m_objectsList.count() == 0) return nullptr;
    return m_objectsList.last();
}

QList<RamObject *> RamObjectList::toList() const
{
    return m_objectsList;
}

void RamObjectList::removeAt(int i)
{
    takeObject(i);
}

void RamObjectList::removeFirst()
{
    if (m_objects.count() > 0) removeAt(0);
}

void RamObjectList::removeLast()
{
    if (m_objects.count() > 0) removeAt(m_objects.count() - 1);
}

RamObject *RamObjectList::takeObject(int i)
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

QList<RamObject*> RamObjectList::removeIndices(QModelIndexList indices)
{
    std::sort(indices.begin(), indices.end(), indexSorter);

    QList<RamObject*> objs;

    for( int i = indices.count() -1; i >= 0; i--)
    {
        QModelIndex index = indices.at(i);
        quintptr iptr = index.data(Qt::UserRole).toULongLong();
        RamObject *o = reinterpret_cast<RamObject*>( iptr );
        objs << o;
        removeAll(o);
    }
    return objs;
}

bool RamObjectList::moveRow(const QModelIndex &sourceParent, int sourceRow, const QModelIndex &destinationParent, int destinationChild)
{
    Q_UNUSED(sourceParent)
    Q_UNUSED(destinationParent)

    beginMoveRows(sourceParent, sourceRow, sourceRow, destinationParent, destinationChild);

    m_objectsList.move(sourceRow, destinationChild);
    m_objectsList.at(destinationChild)->move(destinationChild);

    endMoveRows();
    return true;
}

bool RamObjectList::moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild)
{
    Q_UNUSED(sourceParent)
    Q_UNUSED(destinationParent)

    beginMoveRows(sourceParent, sourceRow, sourceRow + count, destinationParent, destinationChild);

    int offset = 0;
    for (int i = sourceRow; i < sourceRow+count; i++)
    {
        int newRow = destinationChild + offset;
        m_objectsList.move(i, newRow);
        m_objectsList.at(newRow)->move(newRow);
        offset++;
    }

    endMoveRows();
    return true;
}

RamObject *RamObjectList::takeFromUuid(QString uuid)
{
    // get from map
    RamObject *obj = m_objects.value(uuid, nullptr);
    if (!obj) return nullptr;

    // get index from list to remove
    int row = objRow(obj);
    if (row<0) return nullptr;

    return takeObject(row);

    return nullptr;
}

int RamObjectList::count() const
{
    return m_objects.count();
}

bool RamObjectList::contains(RamObject *obj) const
{
    return contains(obj->uuid());
}

bool RamObjectList::contains(QString uuid) const
{
    return m_objects.contains( uuid );
}

void RamObjectList::removeAll(RamObject *obj)
{
    int row = objRow(obj);
    if (row<0) return;

    takeObject(row);
}

void RamObjectList::sort()
{
    sort(0);
}

void RamObjectList::removeAll(QString uuid)
{
    // get from map
    RamObject *obj = m_objects.value(uuid, nullptr);
    if (!obj) return;

    int row = objRow(obj);
    if (row<0) return;

    takeObject(row);
}

bool objectSorter(RamObject *a, RamObject *b)
{
    if (a->order() != b->order()) return a->order() < b->order();
    else return a->shortName() < b->shortName();
}

bool indexSorter(QModelIndex a, QModelIndex b)
{
    return a.row() < b.row();
}
