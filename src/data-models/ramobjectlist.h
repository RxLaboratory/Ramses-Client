#ifndef RAMOBJECTLIST_H
#define RAMOBJECTLIST_H

#include <QAbstractTableModel>
#include <QIcon>
#include <QStringBuilder>

#include "ramabstractobject.h"

/**
 * @brief The RamObjectList class is the base class used for almost all of Ramses' lists (assets, steps, shots, applications...).
 */
template<typename RO> class RamObjectList : public QAbstractTableModel, public RamAbstractObject
{
    Q_OBJECT

public:

    enum DataListMode {
        Object, // a List saved in the RamObject table
        Table, // a complete table
        Temp // temporary list, not saved
    };

    // STATIC METHODS //

    static RamObjectList<RO> *getObject(QString uuid, bool constructNew = false);

    // METHODS //

    RamObjectList(QString shortName, QString name, QObject *parent = nullptr, DataListMode mode = Object);

    // MODEL reimplementation
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    virtual void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;

    // LIST EDITING
    // Empty
    void clear(bool removeObjects = false);
    // Add
    virtual void insertObject(int i, RO obj); // Insert object at i
    void append(RO obj); // Append object
    // Remove
    virtual QList<RO> removeIndices( QModelIndexList indices ); // Used to remove selection. Returns the removed objects
    virtual RO takeObject(int i); // Remove and returns object at i
    RO takeObject(QString uuid); // Remove and return object using uuid
    void removeAll(QString uuid); // Removes object using uuid

    // LIST INFORMATION
    // Info
    bool contains(RO obj) const;
    bool contains(QString uuid) const;
    // Accessors
    RO fromUuid(QString uuid) const;
    RO fromName(QString shortName, QString name = "") const;
    RO at(int i) const;
    RO at(QModelIndex i) const;
    RO last() const;
    RO first() const;

    QList<RO> toList();

public slots:
    void removeAll(RO obj);
    void sort();
    void reload();

signals:
    void objectInserted(RO);
    void objectRemoved(RO);
    void objectDataChanged(RO);

protected:
    RamObjectList(QString uuid, QObject *parent = nullptr);

    virtual QJsonObject reloadData() override;

    // DATA
    // For performance reasons, store both a list and a map
    QMap<QString, RO> m_objects;
    QList<RO> m_objectList;

    // UTILS
    virtual void connectObject(RO obj);
    int objRow(RO obj) const;

private slots:
    // Emits dataChanged() and headerChanged()
    void objectChanged(RO obj);

    /**
     * @brief listChanged monitors the list to save it to the database
     */
    void listChanged();

private:
    void construct(QObject *parent);
    void connectEvents();

    DataListMode m_dataMode = Object;
    QString m_tableName;
};

class RamObject;

bool objectSorter(RamObject *a, RamObject *b);
bool indexSorter(QModelIndex a, QModelIndex b);

#endif // RAMOBJECTLIST_H
