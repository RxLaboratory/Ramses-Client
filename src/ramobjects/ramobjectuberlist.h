#ifndef RAMOBJECTUBERLIST_H
#define RAMOBJECTUBERLIST_H

#include "ramobjectlist.h"

/**
 * @brief The RamObjectUberList class is a RamObjectList of RamObjectList
 * It is used to group different RamObjectLists together
 * and adds methods to get single objects from the lists.
 * It also relays the signals from included lists.
 */
class RamObjectUberList : public RamObjectList
{
    Q_OBJECT
public:
    RamObjectUberList(QObject *parent = nullptr);

    // Single object access inside child lists
    RamObject *objectFromUuid(QString uuid);
    int objectCount();
    bool containsObject(RamObject *obj) const;
    RamObject *objectAt(int index) const;
    void removeObject(QString uuid);
    void removeObject(RamObject *obj);
    RamObject *takeObject(QString uuid);
    // reimplemented to disconnect list
    RamObject *takeAt(int i) Q_DECL_OVERRIDE;

signals:
    void objectAdded(RamObject *, int index);
    void objectRemoved(RamObject *);

protected:
    // reimplemented to connect list
    void addObject(RamObject *obj, int index) Q_DECL_OVERRIDE;

private slots:
    void relayObjectAdded(RamObject *o, int index);
    void relayObjectRemoved(RamObject *o);

private:
    QMap<QString, QList<QMetaObject::Connection>> m_connections;
};

#endif // RAMOBJECTUBERLIST_H
