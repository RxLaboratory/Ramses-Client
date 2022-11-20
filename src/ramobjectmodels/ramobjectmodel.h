#ifndef RAMOBJECTMODEL_H
#define RAMOBJECTMODEL_H

#include "ramabstractobjectmodel.h"

#include "ramobject.h"
#include "ramobjectsortfilterproxymodel.h"

/**
 * @brief The RamObjectModel class represents a list of RamObjects
 */
class RamObjectModel : public RamAbstractObjectModel
{
    Q_OBJECT
public:
    explicit RamObjectModel(RamAbstractObject::ObjectType type, QObject *parent = nullptr);

    // Data Access
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Edit structure
    virtual void insertObjects(int row, QVector<QString> uuids);
    virtual void removeObjects(QStringList uuids);
    //virtual bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    // Columns
    void setColumnModel(RamObjectModel *model);
    RamObjectSortFilterProxyModel *columnModel() const;

    // Support move rows and columns
    virtual bool moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild) override;

    // Convenience access

    // Clear the model
    virtual void clear() override;

    // Append object
    void appendObject(QString uuid);

    // Use one of the data roles for fast lookup
    // Note that this data must be immutable
    void setLookupRole(int role);

    // An object by its role data lookup
    QList<RamObject *> lookUp(QVariant roleData) const;

    // The type of objects contained in this model
    virtual RamObject::ObjectType type() const override;

protected:
    RamObject::ObjectType m_type;

private slots:
    void objectDataChanged(RamObject *obj);
    void columnDataChanged(RamObject *obj);

    // Column changes
    void insertModelColumns(const QModelIndex &parent, int first, int last);
    void removeModelColumns(const QModelIndex &parent, int first, int last);
    void moveModelColumns(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row);

private:
    RamObject *getObject(QString uuid) const;
    void connectObject(QString uuid);
    void connectObject(RamObject *o);
    void disconnectObject(QString uuid);

    int m_lookupRole = RamAbstractObject::ShortName;

    // The data and different ways to access it
    QMultiHash<QVariant,RamObject*> m_lookupTable;
    QVector<RamObject*> m_objects;
    RamObjectSortFilterProxyModel *m_columnObjects;
};

// We need a way to hash QVariant
// (for the types used by RamObjects roleData
uint qHash( const QVariant & var );

#endif // RAMOBJECTMODEL_H
