#ifndef RAMOBJECTLIST_H
#define RAMOBJECTLIST_H

#include <QObject>

#include "ramobject.h"

/**
 * @brief The RamObjectList class represents a list of objects (sequences, shots, etc)
 * It works similarly to a QList
 * And handles assignment/removal, also emitting proper signals
 */
class RamObjectList : public RamObject
{
    Q_OBJECT
public:
    explicit RamObjectList(QObject *parent = nullptr);
    explicit RamObjectList(QString shortName, QString name = "", QString uuid = "", QObject *parent = nullptr);
    RamObjectList(RamObjectList const &other);
    ~RamObjectList();

    virtual RamObject *fromUuid(QString uuid) const;
    virtual RamObject *fromName(QString shortName, QString name = "") const;
    QList<RamObject*> toList() const;

    // QList Methods
    void append(RamObject *obj);
    void insert(int i, RamObject *obj);
    void clear();
    int count() const;
    bool contains(RamObject *obj) const;
    virtual RamObject *at(int i) const;
    void removeAt(int i);
    void removeFirst();
    void removeLast();
    virtual RamObject *takeAt(int i);
    RamObject *takeFromUuid(QString uuid);
    // QList Operators
    virtual RamObject *operator[](int i) const;

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
    virtual void addObject(RamObject *obj, int index);

private:
    QMap<QString, QList<QMetaObject::Connection>> m_connections;
};

bool objectSorter(RamObject *a, RamObject *b);

#endif // RAMOBJECTLIST_H
