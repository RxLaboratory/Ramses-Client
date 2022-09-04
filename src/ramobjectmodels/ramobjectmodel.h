#ifndef RAMOBJECTMODEL_H
#define RAMOBJECTMODEL_H

#include <QAbstractTableModel>

#include "ramobject.h"

/**
 * @brief The RamObjectModel class represents a list of RamObjects
 */
class RamObjectModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum DataRole {
        ShortName = Qt::UserRole+1,
        Name = Qt::UserRole+2,
        Completion = Qt::UserRole+3
    };

    static RamObjectModel *emptyModel();

    explicit RamObjectModel(RamAbstractObject::ObjectType type, QObject *parent = nullptr);

    // Data Access
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Edit structure
    virtual void insertObjects(int row, QStringList uuids);
    virtual void removeObjects(QStringList uuids);
    virtual void insertColumnObjects(int column, QStringList uuids);
    virtual void removeColumnObjects(QStringList uuids);

    // Support move rows and columns
    virtual bool moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild) override;

    // Convenience access
    // An object by its row
    RamObject *get(int row);
    // An object by index
    RamObject *get(QModelIndex index) const;
    // An object by its shortname, or name
    RamObject *search(QString searchString) const;

    RamObject::ObjectType type() const;

protected:
    static RamObjectModel *m_emptyModel;
    RamObject::ObjectType m_type;

private slots:
    void objectDataChanged(RamObject *obj);

private:
    RamObject *getObject(QString uuid) const;
    void connectObject(QString uuid);
    void disconnectObject(QString uuid);
    QVariant objectRoleData(QString uuid, int role) const;

    QStringList m_objectsUuids;
    QStringList m_columnObjectsUuids;
};

#endif // RAMOBJECTMODEL_H
