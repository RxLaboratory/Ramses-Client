#include "ramjsonobjectmodel.h"

#include <QBrush>

#include "ramabstractobject.h"

RamJsonObjectModel::RamJsonObjectModel(QObject *parent)
    : QAbstractListModel{parent}
{}

QVariant RamJsonObjectModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    if (row < 0)
        return QVariant();
    if (row >= _objects.count())
        return QVariant();

    const QJsonObject obj = object(row);

    switch(role) {
    case Qt::DisplayRole:
        return obj.value( RamAbstractObject::KEY_Name
                         ).toString(RamAbstractObject::DEFAULT_Name);
    case Qt::ToolTipRole:
        return obj.value( RamAbstractObject::KEY_Name
                         ).toString(RamAbstractObject::DEFAULT_Name) +
               QStringLiteral("\n") +
               obj.value( RamAbstractObject::KEY_ShortName
                         ).toString(RamAbstractObject::DEFAULT_ShortName);
    case Qt::StatusTipRole:
        return obj.value( RamAbstractObject::KEY_Name
                         ).toString(RamAbstractObject::DEFAULT_Name) +
               QStringLiteral(" | ") +
               obj.value( RamAbstractObject::KEY_ShortName
                         ).toString(RamAbstractObject::DEFAULT_ShortName);
    case Qt::ForegroundRole:
        return QBrush(QColor(
            obj.value( RamAbstractObject::KEY_Color
                      ).toString(RamAbstractObject::DEFAULT_Color)
            ));
    case Qt::EditRole:
        return obj;
    }

    return QVariant();
}

QVariant RamJsonObjectModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (section < 0)
        return QVariant();
    if (section >= _objects.count())
        return QVariant();

    switch(orientation) {
    case Qt::Vertical:
        return object(section).value( RamAbstractObject::KEY_ShortName
                                     ).toString(RamAbstractObject::DEFAULT_ShortName);
    case Qt::Horizontal:
        if (section == 0)
            return tr("Item");
        break;
    }

    return QVariant();
}

bool RamJsonObjectModel::insertRows(int row, int count, const QModelIndex &parent)
{
    Q_ASSERT(row >= 0 && row <= _objects.count());

    beginInsertRows(parent, row, row+count-1);

    for (int i =0; i < count; i++)
        _objects.insert(row, QJsonObject());

    endInsertRows();

    return true;
}

bool RamJsonObjectModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_ASSERT(row >= 0 && row < _objects.count());

    beginRemoveRows(parent, row, row+count-1);

    for (int i = row+count-1; i >= row; i--)
        _objects.removeAt(i);

    endRemoveRows();

    return true;
}

bool RamJsonObjectModel::moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild)
{
    Q_ASSERT(sourceRow >= 0 && sourceRow < _objects.count());
    Q_ASSERT(destinationChild >= 0 && destinationChild <= _objects.count());

    // Nothing to do
    if (sourceRow == destinationChild)
        return true;
    if (sourceRow + 1 == destinationChild)
        return true;

    beginMoveRows(sourceParent, sourceRow, sourceRow+count-1, destinationParent, destinationChild);

    // Move Up
    bool up = destinationChild < sourceRow;

    for (int i = 0; i < count; ++i) {
        if (up)
            _objects.move(sourceRow+i, destinationChild+i);
        else
            _objects.move(sourceRow, destinationChild-1);
    }

    endMoveRows();

    return true;
}

bool RamJsonObjectModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    switch(role) {
    case Qt::DisplayRole: {
        _objects[index.row()].insert(RamAbstractObject::KEY_Name, value.toString());
        emit dataChanged(index, index, {Qt::DisplayRole, Qt::ToolTipRole, Qt::StatusTipRole});
        return true;
    }
    case Qt::ForegroundRole: {
        _objects[index.row()].insert(RamAbstractObject::KEY_Color, value.toString());
        emit dataChanged(index, index, {Qt::ForegroundRole});
        return true;
    }
    case Qt::EditRole: {
        _objects.replace(index.row(), value.toJsonObject());
        emit dataChanged(index,index);
        return true;
    }
    }

    return false;
}
