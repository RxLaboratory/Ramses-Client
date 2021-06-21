#ifndef RAMOBJECTLIST_H
#define RAMOBJECTLIST_H

#include "ramobject.h"

#include <QAbstractTableModel>
#include <QIcon>

class RamObjectList : public QAbstractTableModel
{
    Q_OBJECT
public:
    RamObjectList(QObject *parent = nullptr);
    RamObjectList(QString shortName, QString name = "", QObject *parent = nullptr);

    const QString &shortName() const;
    void setShortName(const QString &newShortName);
    const QString &name() const;
    void setName(const QString &newName);

    // MODEL reimplementation
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    virtual void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) Q_DECL_OVERRIDE;

    // MODEL EDITING
    void clear(); // Reset
    virtual void insertObject(int i, RamObject *obj); // Insert Row
    virtual RamObject *takeObject(int i); // Remove Row
    virtual QList<RamObject *> removeIndices( QModelIndexList indices ); // Used to remove selection. Returns the removed objects

    // CONVENIENCE METHODS
    // Info
    int count() const;
    bool contains(RamObject *obj) const;
    bool contains(QString uuid) const;
    // Accessors
    RamObject *fromUuid(QString uuid) const;
    RamObject *fromName(QString shortName, QString name = "") const;
    RamObject *at(int i) const;
    RamObject *last() const;
    QList<RamObject*> toList() const;
    // Insertion
    void append(RamObject *obj);
    // Removal
    void removeAt(int i);
    void removeFirst();
    void removeLast();
    void removeAll(QString uuid);
    RamObject *takeFromUuid(QString uuid);

public slots:
    void removeAll(RamObject *obj);
    void sort();

protected:
    // DATA
    // For performance reasons, store both a list and a map
    QMap<QString, RamObject*> m_objects;
    QList<RamObject*> m_objectsList;

    // Object connections (to detect changes)
    QMap<QString, QList<QMetaObject::Connection>> m_connections;

    // UTILS
    virtual void connectObject(RamObject *obj);
    int objRow(RamObject *obj);

private slots:
    // Emits dataChanged() and headerChanged()
    void objectChanged(RamObject *obj);

private:
    QString m_shortName;
    QString m_name;
};

bool objectSorter(RamObject *a, RamObject *b);
bool indexSorter(QModelIndex a, QModelIndex b);

#endif // RAMOBJECTLIST_H