#include "ramobjectlist.h"

RamObjectList::RamObjectList(QObject *parent) :
    RamObject(parent)
{
    this->setObjectName( "RamObjectList" );
    this->setObjectType(ObjectList);
    m_model = new RamObjectListModel();
}

RamObjectList::RamObjectList(QString shortName, QString name, QString uuid, QObject *parent) :
    RamObject(shortName, name, uuid, parent)
{
    this->setObjectName( "RamObjectList" );
    this->setObjectType(ObjectList);
    m_model = new RamObjectListModel();
}

RamObjectList *RamObjectList::objectList(QString uuid)
{
    return qobject_cast<RamObjectList*>( RamObject::obj(uuid) );
}

RamObjectListModel *RamObjectList::model() const
{
    return m_model;
}

