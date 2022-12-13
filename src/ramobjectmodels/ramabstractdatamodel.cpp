#include "ramabstractdatamodel.h"

RamAbstractDataModel::RamAbstractDataModel()
{

}

RamObject *RamAbstractDataModel::get(const QModelIndex &index)
{
    quintptr iptr = index.data(RamObject::Pointer).toULongLong();
    if (iptr == 0) return nullptr;
    return reinterpret_cast<RamObject*>( iptr );
}

QSet<RamObject *> RamAbstractDataModel::intersectLookUp(QString lookUpKey, QString lookUpValue, QSet<RamObject *> objList) const
{
    return objList.intersect( lookUp(lookUpKey, lookUpValue) );
}
