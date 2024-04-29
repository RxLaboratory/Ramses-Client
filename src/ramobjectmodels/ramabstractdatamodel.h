#ifndef RAMABSTRACTDATAMODEL_H
#define RAMABSTRACTDATAMODEL_H

#include "ramobject.h"

class RamAbstractDataModel
{
public:
    RamAbstractDataModel();

    // === Data Access ===

    // An object by ModelIndex
    static RamObject *get(const QModelIndex &index);

    // Number of objects
    virtual int count() const = 0;

    // A Uuid
    virtual QString getUuid(int row) const = 0;
    // An object by row
    virtual RamObject *get(int row) const = 0;
    // An object by its shortname, or name
    virtual RamObject *search(QString searchString) const = 0;
    // Objects by their lookup key
    virtual QSet<RamObject *> lookUp(QString lookUpKey, QString lookUpValue) const = 0;
    // Modifies the list and returns a reference to it
    virtual QSet<RamObject *> &intersectLookUp(QString lookUpKey, QString lookUpValue, QSet<RamObject *> &objList) const;

    // All the uuids
    virtual QVector<QString> toVector() const = 0;
    virtual QStringList toStringList() const = 0;

    // Check if contains
    virtual bool contains(QString uuid) const = 0;

    // The type of objects
    virtual RamObject::ObjectType type() const = 0;

    virtual int objectRow(RamObject *obj) const = 0;
    virtual int uuidRow(QString uuid) const = 0;
};

#endif // RAMABSTRACTDATAMODEL_H
