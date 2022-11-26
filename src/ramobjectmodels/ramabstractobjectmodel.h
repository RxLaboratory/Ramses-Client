#ifndef RAMABSTRACTOBJECTMODEL_H
#define RAMABSTRACTOBJECTMODEL_H

#include "ramabstractdatamodel.h"

/**
 * @brief The RamAbstractObjectModel class is used to store and get access to the underlying data of RamObjectModel and DBTableModel
 * It does not reimplement anything from QAbstractTableModel, but just provides ways to access the actual data.
 */
class RamAbstractObjectModel : public QAbstractTableModel, public RamAbstractDataModel
{
public:
    static RamAbstractObjectModel *emptyModel();

    explicit RamAbstractObjectModel(RamObject::ObjectType type, QObject *parent = nullptr);

    // === Parameters ===

    /**
     * @brief addLookUpKey Sets a key of the JSON Data to be used for fast look ups
     * The default is to use the shortName
     * @param newLookUpKey The key
     */
    void addLookUpKey(const QString &newLookUpKey);

    // === Data Access ===

    // Number of objects
    virtual int count() const override;

    // A Uuid
    virtual QString getUuid(int row) const override;
    // An object by row
    virtual RamObject *get(int row) const override;

    // An object by its shortname, or name
    virtual RamObject *search(QString searchString) const override;
    // Objects by their lookup key
    virtual QSet<RamObject *> lookUp(QString lookUpKey, QString lookUpValue) const override;

    // All the uuids
    virtual QVector<QString> toVector() const override;
    virtual QStringList toStringList() const override;

    // Check if contains
    virtual bool contains(QString uuid) const override;

    // The type of objects
    virtual RamObject::ObjectType type() const override;

    // The row of an object
    virtual int objectRow(RamObject *obj) const override;
    virtual int uuidRow(QString uuid) const override;

protected:

    // === Data Edition ===

    virtual void clear();
    void insertObject(int row, QString uuid, QString data);
    void removeObject(QString uuid);
    void updateObject(QString uuid, QString data);
    void moveObjects(int from, int count, int to);

    // Gets the lookUp key value from the data
    QString getLookUpValue(QString key, QString data);
    // Remove an object from the lookup table
    void removeObjectFromLookUp(QString uuid);

    // === Attributes ===

    static RamAbstractObjectModel *m_emptyModel;

    // === The Data ===

    // All Uuids, sorted
    QStringList m_objectUuids;
    // Fast LookUp table (key / uuid)
    QHash<QString, QMultiHash<QString, QString>> m_lookUpTables;

    // === Settings ===

    // The table name
    QString m_table;
};

#endif // RAMABSTRACTOBJECTMODEL_H
