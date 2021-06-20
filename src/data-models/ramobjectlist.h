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
    virtual void sort(int column = 0, Qt::SortOrder order = Qt::AscendingOrder) Q_DECL_OVERRIDE;

    // MODEL EDITING
    void clear(); // Reset
    // TODO header data on object insert and change
    virtual void insertObject(int i, RamObject *obj); // Insert Row
    virtual RamObject *takeObject(int i); // Remove Row
    //virtual void sort(); // Sort

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
    // Insertion
    void append(RamObject *obj);
    // Removal
    void removeAt(int i);
    void removeFirst();
    void removeLast();
    void removeAll(RamObject *obj);
    void removeAll(QString uuid);
    RamObject *takeFromUuid(QString uuid);

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
    // Emits dataChanged()
    void objectChanged(RamObject *obj);

private:
    QString m_shortName;
    QString m_name;
};

bool objectSorter(RamObject *a, RamObject *b);

#endif // RAMOBJECTLIST_H
