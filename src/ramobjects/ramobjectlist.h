#ifndef RAMOBJECTLIST_H
#define RAMOBJECTLIST_H

#include <QObject>

#include "ramobject.h"

class RamObjectList : public RamObject
{
    Q_OBJECT
public:
    explicit RamObjectList(QObject *parent = nullptr);
    explicit RamObjectList(QString shortName, QString name = "", QString uuid = "", QObject *parent = nullptr);
    RamObjectList(RamObjectList const &other);
    ~RamObjectList();

    RamObject *fromUuid(QString uuid) const;
    QList<RamObject*> toList() const;

    // QList Methods
    void append(RamObject *obj);
    void insert(int i, RamObject *obj);
    void clear();
    int count() const;
    bool contains(RamObject *obj) const;
    RamObject *at(int i);
    void removeAt(int i);
    void removeFirst();
    void removeLast();
    RamObject *takeAt(int i);
    // QList Operators
    RamObject *operator[](int i) const;

public slots:
    void removeAll(RamObject *obj);
    void removeAll(QString uuid);
    virtual void sort();

signals:
    void objectAdded(RamObject *, int index);
    void objectRemoved(RamObject *);
    void cleared();

protected:
    QList<RamObject*> m_objects;

private:
    QMap<QString, QList<QMetaObject::Connection>> m_connections;

    void addObject(RamObject *obj, int index);
};

bool objectSorter(RamObject *a, RamObject *b);

#endif // RAMOBJECTLIST_H
