#ifndef RAMOBJECTLIST_H
#define RAMOBJECTLIST_H

#include <QAbstractTableModel>
#include <QIcon>
#include <QStringBuilder>

#include "ramabstractobject.h"
#include "ramobject.h"

/**
 * @brief The RamObjectList class is the base class used for almost all of Ramses' lists (assets, steps, shots, applications...).
 */
class RamObjectList : public QAbstractTableModel, public RamAbstractObject
{
    Q_OBJECT

public:

    enum DataListMode {
        ListObject, // a List saved in the RamObject table
        Table, // a complete table
        Temp // temporary list, not saved
    };

    enum DataRole {
        Pointer = Qt::UserRole,
        DefaultSortOrder = Qt::UserRole + 1,
        ItemShortName = Qt::UserRole + 2,
        ItemName = Qt::UserRole + 3,
        Difficulty = Qt::UserRole + 4,
        TimeSpent = Qt::UserRole + 5,
        Estimation = Qt::UserRole + 6,
        Completion = Qt::UserRole + 7,
        IsPM = Qt::UserRole + 8,
        UUID = Qt::UserRole + 9,
        Date = Qt::UserRole + 10,
        IsComment = Qt::UserRole + 11,
        Comment = Qt::UserRole +12
    };

    // STATIC METHODS //

    static RamObjectList *get(QString uuid, ObjectType type);
    static RamObjectList *c(QObject *obj);
    static RamObjectList *emptyList();
    static RamObject *at(QModelIndex i);

    // METHODS //

    RamObjectList(QString shortName, QString name, ObjectType type, DataListMode mode, QObject *parent = nullptr, ObjectType listType = ObjectList);
    RamObjectList(QString uuid, QObject *parent = nullptr, ObjectType listType = ObjectList);

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
    virtual void insertObject(int i, RamObject *obj); // Insert object at i
    void append(RamObject *obj); // Append object
    // Remove
    virtual QList<RamObject *> removeIndices( QModelIndexList indices ); // Used to remove selection. Returns the removed objects
    virtual RamObject *takeObject(int i); // Remove and returns object at i
    RamObject *takeObject(QString uuid); // Remove and return object using uuid
    void removeAll(QString uuid); // Removes object using uuid

    // LIST INFORMATION
    // Info
    bool contains(RamObject *obj) const;
    bool contains(QString uuid) const;
    // Accessors
    RamObject *fromUuid(QString uuid) const;
    RamObject *fromName(QString shortName, QString name = "") const;
    RamObject *at(int i) const;
    RamObject *last() const;
    RamObject *first() const;

    QList<RamObject *> toList();

public slots:
    void removeAll(RamObject *obj);
    void sort();
    void reload();

signals:
    // Emitted when rows inserted, removed or reordered
    void listChanged(RamObjectList*);

protected:
    // An empty list is useful
    static RamObjectList *m_emptyList;

    // DATA
    // For performance reasons, store both a list and a map
    QMap<QString, RamObject*> m_objects;
    QList<RamObject*> m_objectList;

    // UTILS
    int objRow(RamObject *obj) const;

    ObjectType m_contentType;

protected slots:
    // Emits dataChanged() and headerChanged()
    void objectChanged(RamObject *obj);

    virtual QJsonObject reloadData() override;
    virtual void saveData();

private:
    void construct(QObject *parent);
    void connectEvents();
    void connectObject(RamObject *obj);

    DataListMode m_dataMode = ListObject;
};

class RamObject;

bool objectSorter(RamObject *a, RamObject *b);
bool indexSorter(QModelIndex a, QModelIndex b);

#endif // RAMOBJECTLIST_H
