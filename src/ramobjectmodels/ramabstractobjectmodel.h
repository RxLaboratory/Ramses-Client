#ifndef RAMABSTRACTOBJECTMODEL_H
#define RAMABSTRACTOBJECTMODEL_H

#include "ramobject.h"

class RamAbstractObjectModel : public QAbstractTableModel
{
public:
    static RamAbstractObjectModel *emptyModel();

    explicit RamAbstractObjectModel(RamObject::ObjectType type, QObject *parent = nullptr);

    // Common methods
    virtual void clear();

    // An object by its row
    virtual RamObject *get(int row) const;
    // An object by ModelIndex
    static RamObject *get(const QModelIndex &index);
    // An object by its shortname, or name
    virtual RamObject *search(QString searchString) const = 0;

    // All the uuids
    QVector<QString> toVector() const;
    QStringList toStringList() const;

    // Check if contains
    virtual bool contains(QString uuid) const;

    // The type of objects
    RamObject::ObjectType type() const;

protected:
    static RamAbstractObjectModel *m_emptyModel;

    // The data
    QStringList m_objectUuids;
    QString m_table;
};

#endif // RAMABSTRACTOBJECTMODEL_H
