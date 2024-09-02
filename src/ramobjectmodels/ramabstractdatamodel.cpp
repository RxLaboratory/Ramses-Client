#include "ramabstractdatamodel.h"

#include "qabstractitemmodel.h"

RamAbstractDataModel::RamAbstractDataModel()
{

}

RamObject *RamAbstractDataModel::get(const QModelIndex &index)
{
    quintptr iptr = index.data(RamObject::Pointer).toULongLong();
    if (iptr == 0) return nullptr;
    return reinterpret_cast<RamObject*>( iptr );
}

QSet<RamObject *> &RamAbstractDataModel::intersectLookUp(QString lookUpKey, QString lookUpValue, QSet<RamObject *> &objList) const
{
    // Don't use another lookup and QSet::intersect,
    // as it would be slower
    // but actually iterate throught the existing set
    QMutableSetIterator<RamObject*> i(objList);
    while(i.hasNext()) {
        i.next();
        RamObject *obj = i.value();
        if (obj->getData(lookUpKey).toString() != lookUpValue)
            objList.remove(obj);
    }

    return objList;
}
