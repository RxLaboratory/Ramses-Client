#ifndef DBTABLEMODEL_H
#define DBTABLEMODEL_H

#include "ramobjectmodel.h"

/**
 * @brief The DBTableModel class handles a list of objects taken from a complete table in the DB
 */
class DBTableModel: public RamObjectModel
{
    Q_OBJECT
public:
    DBTableModel(RamAbstractObject::ObjectType type, QObject *parent = nullptr);

private slots:
    void insertObject(QString uuid, QString table);
    void removeObject(QString uuid, QString table);
};

#endif // DBTABLEMODEL_H
