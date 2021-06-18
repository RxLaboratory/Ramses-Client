#ifndef RAMOBJECTLISTMODEL_H
#define RAMOBJECTLISTMODEL_H

#include "ramobject.h"

#include <QAbstractListModel>
#include <QIcon>

class RamObjectListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    RamObjectListModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    RamObject *fromUuid(QString uuid) const;
    RamObject *fromName(QString shortName, QString name = "") const;

    // QList Methods
    void append(RamObject *obj);
    void insert(int i, RamObject *obj);
    void clear();
    int count() const;
    bool contains(RamObject *obj) const;
    bool contains(QString uuid) const;
    virtual RamObject *at(int i) const;
    RamObject *last() const;
    void removeAt(int i);
    void removeFirst();
    void removeLast();
    virtual RamObject *takeAt(int i);
    RamObject *takeFromUuid(QString uuid);
    void removeAll(RamObject *obj);
    void removeAll(QString uuid);
    //virtual void sort();

protected:
    // For performance reasons, store both a list and a map
    QMap<QString, RamObject*> m_objects;
    QList<RamObject*> m_objectsList;

private slots:
    void objectChanged(RamObject *obj);

private:
    QMap<QString, QList<QMetaObject::Connection>> m_connections;

    void connectObject(RamObject *obj);

    // utils
    int objRow(RamObject *obj);
};

bool objectSorter(RamObject *a, RamObject *b);

#endif // RAMOBJECTLISTMODEL_H
