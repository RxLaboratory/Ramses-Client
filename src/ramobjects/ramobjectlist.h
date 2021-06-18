#ifndef RAMOBJECTLIST_H
#define RAMOBJECTLIST_H

#include <QObject>
#include <QMapIterator>

#include "ramobject.h"
#include "ramobjectlistmodel.h"

/**
 * @brief The RamObjectList class represents a list of objects (sequences, shots, etc)
 * It works similarly to a QList
 * And handles assignment/removal, also emitting proper signals
 */
class RamObjectList : public RamObject
{
    Q_OBJECT
public:
    explicit RamObjectList(QObject *parent = nullptr);
    explicit RamObjectList(QString shortName, QString name = "", QString uuid = "", QObject *parent = nullptr);

    RamObjectListModel *model() const;

    static RamObjectList *objectList( QString uuid );

protected:
    RamObjectListModel *m_model;

};

bool objectSorter(RamObject *a, RamObject *b);

#endif // RAMOBJECTLIST_H
