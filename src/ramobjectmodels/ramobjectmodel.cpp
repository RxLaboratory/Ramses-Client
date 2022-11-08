#include "ramobjectmodel.h"
#include "ramstatus.h"
#include "ramstep.h"
#include "ramabstractitem.h"

RamObjectModel *RamObjectModel::m_emptyModel = nullptr;

RamObjectModel *RamObjectModel::emptyModel()
{
    if (!m_emptyModel) m_emptyModel = new RamObjectModel(RamObject::Object);
    return m_emptyModel;
}

RamObjectModel::RamObjectModel(RamAbstractObject::ObjectType type, QObject *parent)
    : QAbstractTableModel{parent}
{
    m_type = type;
    m_columnObjects = new RamObjectSortFilterProxyModel();
    connect(m_columnObjects, &RamObjectModel::rowsInserted, this, &RamObjectModel::insertModelColumns);
    connect(m_columnObjects, &RamObjectModel::rowsAboutToBeMoved, this, &RamObjectModel::removeModelColumns);
    connect(m_columnObjects, &RamObjectModel::rowsMoved, this, &RamObjectModel::moveModelColumns);
}

void RamObjectModel::setLookupRole(int role)
{
    m_lookupRole = role;
}

int RamObjectModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_objectUuids.count();
}

int RamObjectModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1 + m_columnObjects->rowCount();
}

QVariant RamObjectModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    int col = index.column();

    QString uuid = m_objectUuids.at(row);
    RamObject *obj = m_objects.at(row);

    if (role == RamObject::Pointer) {
        if (obj) return reinterpret_cast<quintptr>(obj);
        return 0;
    }

    if (!obj) return uuid;

    if (col == 0) return obj->roleData(role);

    // For other columns

    QString colUuid = m_columnObjects->data(
                m_columnObjects->index(col-1, 0),
                RamObject::UUID
                ).toString();
    RamObject *colObj = obj->objectForColumn(colUuid);
    if (!colObj) {
        // Return info from a nostatus
        RamAbstractItem *item = qobject_cast<RamAbstractItem*>( obj );
        if (!item) return QVariant();
        QString stepUuid = headerData(col, Qt::Horizontal, RamObject::UUID).toString();
        if (stepUuid == "") return QVariant();
        RamStep *step = RamStep::get( stepUuid );
        if (!step) return QVariant();
        RamStatus *status = RamStatus::noStatus(item, step);
        if (status) return status->roleData(role);
        return QVariant();
    }
    return colObj->roleData(role);
}

QVariant RamObjectModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (section < 0) return QVariant();
    if (orientation == Qt::Horizontal && section >= columnCount()) return QVariant();
    if (orientation == Qt::Vertical && section >= rowCount()) return QVariant();

    // Vertical
    if (orientation == Qt::Vertical)
    {
        QString uuid = m_objectUuids.at(section);
        if (role == Qt::DisplayRole) {
            RamObject *obj = getObject(uuid);
            if (obj) return obj->roleData(RamObject::ShortName);
            else return "";
        }
        if (role == RamObject::Pointer) {
            RamObject *obj = getObject(uuid);
            if (obj) return reinterpret_cast<quintptr>(obj);
            return 0;
        }
        if (role == RamObject::UUID) return uuid;
        return QAbstractTableModel::headerData(section, orientation, role);
    }

    // Horizontal
    if (section > 0)
    {
        return m_columnObjects->headerData(section-1, Qt::Vertical, role);
    }

    // Horizontal first column
    return QVariant();
}

void RamObjectModel::insertObjects(int row, QVector<QString> uuids)
{
    // Check uuids first, and retrieve the objects
    QVector<RamObject*> objs;
    for (int i = 0; i <  uuids.count(); i++)
    {
        QString uuid = uuids.at(i);
        if (uuid == "") continue;
        RamObject *o = getObject(uuid);
        if (!o) continue;
        if (!o->isValid()) continue;
        if (o->isRemoved()) continue;
        objs << o;
    }

    beginInsertRows(QModelIndex(), row, row + objs.count()-1);

    for (int i = objs.count()-1; i >= 0; i--)
    {
        RamObject *o = objs.at(i);
        // Add to the uuid list
        m_objectUuids.insert(row, o->uuid());
        // Add to the object list
        m_objects.insert(row, o);
        // Add to the lookup table
        m_lookupTable.insert( o->roleData(m_lookupRole), o );

        connectObject( o );
    }

    endInsertRows();
}

void RamObjectModel::removeObjects(QStringList uuids)
{
    // TODO maybe group calls to batch remove contiguous rows
    // if there are performance issues
    // beginRemoveRows can take a group of rows
    while (!uuids.isEmpty())
    {
        QString uuid = uuids.takeLast();
        int i = m_objectUuids.indexOf(uuid);
        if (i>=0) disconnectObject(uuid);
        while( i >= 0 )
        {
            beginRemoveRows(QModelIndex(), i, i);

            // Remove from lookup table
            RamObject *o = getObject(uuid);
            m_lookupTable.remove( o->roleData(m_lookupRole), o );
            // Remove from uuid list
            m_objectUuids.removeAt(i);
            // Remove from object list
            m_objects.removeAt(i);

            endRemoveRows();
            i = m_objectUuids.indexOf(uuid);
        }
    }
}

void RamObjectModel::setColumnModel(RamObjectModel *model)
{
    beginResetModel();
    m_columnObjects->setSourceModel(model);
    endResetModel();
}

RamObjectSortFilterProxyModel *RamObjectModel::columnModel() const
{
    return m_columnObjects;
}

bool RamObjectModel::moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild)
{
    Q_UNUSED(sourceParent)
    Q_UNUSED(destinationParent)

    int sourceEnd = sourceRow + count-1;
    int d = destinationChild;
    if (sourceRow < destinationChild) d++;

    if (!beginMoveRows(QModelIndex(), sourceRow, sourceEnd, QModelIndex(), d))
        return false;

    for (int i = 0; i < count ; i++)
    {
        if (destinationChild < sourceRow) {
            m_objectUuids.move(sourceEnd, destinationChild);
            m_objects.move(sourceEnd, destinationChild);
        }
        else {
            m_objectUuids.move(sourceRow, destinationChild);
            m_objects.move(sourceRow, destinationChild);
        }
    }

    endMoveRows();

    return true;
}

void RamObjectModel::clear()
{
    if (rowCount() == 0) return;

    beginResetModel();

    while(m_objectUuids.count() > 0)
    {
        disconnectObject( m_objectUuids.takeLast() );
    }

    endResetModel();
}

void RamObjectModel::appendObject(QString uuid)
{
    if (m_objectUuids.contains(uuid)) return;

    insertObjects(
                rowCount(),
                QVector<QString>() << uuid
                );
}

RamObject *RamObjectModel::get(int row)
{
    if (row < 0) return nullptr;
    if (row >= m_objects.count()) return nullptr;
    return m_objects.at(row);
}

RamObject *RamObjectModel::get(const QModelIndex &index)
{
    quintptr iptr = index.data(RamObject::Pointer).toULongLong();
    if (iptr == 0) return nullptr;
    return reinterpret_cast<RamObject*>( iptr );
}

RamObject *RamObjectModel::search(QString searchString) const
{
    // Shortname first
    for (int i = 0; i < m_objectUuids.count(); i++)
    {
        QString uuid = m_objectUuids.at(i);
        RamObject *o = RamObject::get(uuid, m_type);
        if (!o) continue;
        if (o->shortName() == searchString) return o;
    }
    // Name after
    for (int i = 0; i < m_objectUuids.count(); i++)
    {
        QString uuid = m_objectUuids.at(i);
        RamObject *o = RamObject::get(uuid, m_type);
        if (!o) continue;
        if (o->name() == searchString) return o;
    }
    return nullptr;
}

QList<RamObject*> RamObjectModel::get(QVariant roleData)
{
    return m_lookupTable.values(roleData);
}

bool RamObjectModel::contains(QString uuid) const
{
    return m_objectUuids.contains(uuid);
}

RamObject::ObjectType RamObjectModel::type() const
{
    return m_type;
}

QVector<QString> RamObjectModel::toVector() const
{
    return m_objectUuids;
}

QStringList RamObjectModel::toStringList() const
{
    QStringList l = m_objectUuids.toList();
    return l;
}

void RamObjectModel::objectDataChanged(RamObject *obj)
{
    QString uuid = obj->uuid();
    // Get the coordinates
    int row = m_objectUuids.indexOf(uuid);
    if (row >= 0 && row < m_objectUuids.count())
    {
        QModelIndex i = index(row, 0);
        QModelIndex iEnd = index(row, columnCount() -1);
        emit dataChanged(i, iEnd);
        emit headerDataChanged(Qt::Vertical, row, row);
        return;
    }
}

void RamObjectModel::columnDataChanged(RamObject *obj)
{
    for (int i = 0; i < m_columnObjects->rowCount(); i++)
    {
        if (obj->is( m_columnObjects->get(i)))
        {
            int col = i+1;
            QModelIndex iStart = index(0, col);
            QModelIndex iEnd = index(rowCount() - 1, col);
            emit dataChanged(iStart, iEnd);
            emit headerDataChanged(Qt::Horizontal, col, col);
            return;
        }
    }
}

void RamObjectModel::insertModelColumns(const QModelIndex &parent, int first, int last)
{
    beginInsertColumns(parent, first+1, last+1);
    for (int i = first; i <= last; i++) {
        RamObject *obj = m_columnObjects->get(i);
        connect(obj, &RamObject::dataChanged, this, &RamObjectModel::columnDataChanged);
    }
    endInsertColumns();
}

void RamObjectModel::removeModelColumns(const QModelIndex &parent, int first, int last)
{
    beginRemoveColumns(parent, first+1, last+1);
    for (int i = first; i <= last; i++) {
        RamObject *obj = m_columnObjects->get(i);
        if (obj) disconnect(obj, nullptr, this, nullptr);
    }
    endRemoveColumns();
}

void RamObjectModel::moveModelColumns(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row)
{
    beginMoveColumns(parent, start+1, end+1, destination, row);
    endMoveColumns();
}

RamObject *RamObjectModel::getObject(QString uuid) const
{
    return RamObject::get(uuid, m_type);
}

void RamObjectModel::connectObject(QString uuid)
{
    RamObject *obj = getObject( uuid );
    connectObject(obj);
}

void RamObjectModel::connectObject(RamObject *o)
{
    connect(o, &RamObject::dataChanged, this, &RamObjectModel::objectDataChanged);
}

void RamObjectModel::disconnectObject(QString uuid)
{
    RamObject *obj = getObject( uuid );
    if (obj) disconnect(obj, nullptr, this, nullptr);
}

uint qHash(const QVariant &var)
{
    if ( !var.isValid() || var.isNull() )
        return -1;

   switch ( var.type() )
   {
       case QVariant::Int:
               return qHash( var.toInt() );
           break;
       case QVariant::UInt:
               return qHash( var.toUInt() );
           break;
       case QVariant::Bool:
               return qHash( var.toUInt() );
           break;
       case QVariant::Double:
               return qHash( var.toUInt() );
           break;
       case QVariant::LongLong:
               return qHash( var.toLongLong() );
           break;
       case QVariant::ULongLong:
               return qHash( var.toULongLong() );
           break;
       case QVariant::String:
               return qHash( var.toString() );
           break;
       case QVariant::Char:
               return qHash( var.toChar() );
           break;
       case QVariant::StringList:
               return qHash( var.toString() );
           break;
       case QVariant::ByteArray:
               return qHash( var.toByteArray() );
           break;
       case QVariant::Date:
       case QVariant::Time:
       case QVariant::DateTime:
       case QVariant::Url:
       case QVariant::Locale:
       case QVariant::RegExp:
               return qHash( var.toString() );
           break;
       case QVariant::Map:
       case QVariant::List:
       case QVariant::BitArray:
       case QVariant::Size:
       case QVariant::SizeF:
       case QVariant::Rect:
       case QVariant::LineF:
       case QVariant::Line:
       case QVariant::RectF:
       case QVariant::Point:
       case QVariant::PointF:
           // not supported yet
           break;
       case QVariant::UserType:
       case QVariant::Invalid:
       default:
           return -1;
   }

   // could not generate a hash for the given variant
   return -1;
}
