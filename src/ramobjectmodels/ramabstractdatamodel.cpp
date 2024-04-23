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

QSet<RamObject *> RamAbstractDataModel::multiLookUp(const QStringList &lookUpKeys, const QStringList &lookUpValues) const
{
    Q_ASSERT(!lookUpKeys.isEmpty());
    Q_ASSERT(lookUpKeys.count() == lookUpValues.count());

    QSet<RamObject *> objs = lookUp(
        lookUpKeys.first(),
        lookUpValues.first()
        );

    if (lookUpKeys.count() <= 1)
        return objs;

    for (int i = 1; i < lookUpKeys.count(); ++i) {
        objs.intersect(
            lookUp(
                lookUpKeys.at(i),
                lookUpValues.at(i)
                )
            );
    }

    return objs;
}

QSet<RamObject *> RamAbstractDataModel::intersectLookUp(QString lookUpKey, QString lookUpValue, QSet<RamObject *> objList) const
{
    return objList.intersect( lookUp(lookUpKey, lookUpValue) );
}
