#include "ramabstractobjectmodel.h"
#include "ramobjectmodel.h"

RamAbstractObjectModel *RamAbstractObjectModel::m_emptyModel = nullptr;

RamAbstractObjectModel *RamAbstractObjectModel::emptyModel()
{
    if (!m_emptyModel) m_emptyModel = new RamObjectModel(RamObject::Object);
    return m_emptyModel;
}

RamAbstractObjectModel::RamAbstractObjectModel(RamAbstractObject::ObjectType type, QObject *parent)
    : QAbstractTableModel{parent}
{
    m_table = RamAbstractObject::objectTypeName(type);
}

void RamAbstractObjectModel::clear()
{
    m_objectUuids.clear();
}

RamObject *RamAbstractObjectModel::get(int row) const
{
    if (row < 0) return nullptr;
    if (row >= rowCount()) return nullptr;

    QString uuid = m_objectUuids.at(row);
    if (uuid == "") return nullptr;

    return RamObject::get(uuid, m_table);
}

RamObject *RamAbstractObjectModel::get(const QModelIndex &index)
{
    quintptr iptr = index.data(RamObject::Pointer).toULongLong();
    if (iptr == 0) return nullptr;
    return reinterpret_cast<RamObject*>( iptr );
}

QVector<QString> RamAbstractObjectModel::toVector() const
{
    return m_objectUuids.toVector();
}

QStringList RamAbstractObjectModel::toStringList() const
{
    return m_objectUuids;
}

bool RamAbstractObjectModel::contains(QString uuid) const
{
    return m_objectUuids.contains(uuid);
}

RamAbstractObject::ObjectType RamAbstractObjectModel::type() const
{
    return RamObject::objectTypeFromName( m_table );
}
