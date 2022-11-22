#include "ramabstractobjectmodel.h"

#include "ramobjectmodel.h"

RamAbstractObjectModel *RamAbstractObjectModel::m_emptyModel = nullptr;

RamAbstractObjectModel *RamAbstractObjectModel::emptyModel()
{
    if (!m_emptyModel) m_emptyModel = new RamObjectModel(RamObject::Object);
    return m_emptyModel;
}

RamAbstractObjectModel::RamAbstractObjectModel(RamAbstractObject::ObjectType type, QObject *parent)
    : QAbstractTableModel{parent}
{
    m_table = RamAbstractObject::objectTypeName(type);
    addLookUpKey("shortName");
}

void RamAbstractObjectModel::addLookUpKey(const QString &newLookUpKey)
{
    if (!m_lookUpTables.contains(newLookUpKey))
        m_lookUpTables.insert(newLookUpKey, QMultiHash<QString, QString>());
}

int RamAbstractObjectModel::count() const
{
    return rowCount();
}

QString RamAbstractObjectModel::getUuid(int row) const
{
    if (row < 0) return "";
    if (row >= rowCount()) return "";
    return m_objectUuids.at(row);
}

RamObject *RamAbstractObjectModel::get(int row) const
{
    QString uuid = getUuid(row);
    if (uuid == "") return nullptr;
    return RamObject::get(uuid, type());
}

void RamAbstractObjectModel::clear()
{
    m_objectUuids.clear();
    // Clear lookup tables
    QHash<QString, QMultiHash<QString, QString>>::iterator i = m_lookUpTables.begin();
    while (i != m_lookUpTables.end())
    {
        i.value().clear();
        i++;
    }
}

RamObject *RamAbstractObjectModel::search(QString searchString) const
{
    // Shortname
    // Try with the lookup table first
    QMultiHash<QString, QString> lookUpTable = m_lookUpTables.value("shortName");
    if (!lookUpTable.isEmpty())
    {
        QStringList uuids = lookUpTable.values(searchString);
        for(int i = 0; i < uuids.count(); i++)
        {
            QString uuid = uuids.at(i);
            if (uuid == "") continue;
            RamObject *o = RamObject::get(uuid, m_table);
            if (!o) continue;
            if (o->shortName() == searchString) return o;
        }
    }
    for (int i = 0; i < m_objectUuids.count(); i++)
    {
        QString uuid = m_objectUuids.at(i);
        if (uuid == "") continue;
        RamObject *o = RamObject::get(uuid, m_table);
        if (!o) continue;
        if (o->shortName() == searchString) return o;
    }
    // Name
    // Try with the lookup table first
    lookUpTable = m_lookUpTables.value("name");
    if (!lookUpTable.isEmpty())
    {
        QStringList uuids = lookUpTable.values(searchString);
        for(int i = 0; i < uuids.count(); i++)
        {
            QString uuid = uuids.at(i);
            if (uuid == "") continue;
            RamObject *o = RamObject::get(uuid, m_table);
            if (!o) continue;
            if (o->name() == searchString) return o;
        }
    }
    for (int i = 0; i < m_objectUuids.count(); i++)
    {
        QString uuid = m_objectUuids.at(i);
        if (uuid == "") continue;
        RamObject *o = RamObject::get(uuid, m_table);
        if (!o) continue;
        if (o->name() == searchString) return o;
    }
    return nullptr;
}

QSet<RamObject *> RamAbstractObjectModel::lookUp(QString lookUpKey, QString lookUpValue) const
{
    QMultiHash<QString, QString> lookUpTable = m_lookUpTables.value(lookUpKey);
    if (lookUpTable.isEmpty()) return QSet<RamObject *>();

    QList<QString> uuids = lookUpTable.values(lookUpValue);
    QSet<RamObject *> objs;
    for (int i = 0; i < uuids.count(); i++)
    {
        QString uuid = uuids.at(i);
        if (uuid == "") continue;
        RamObject *o = RamObject::get(uuid, m_table);
        if (!o) continue;
        objs << o;
    }
    return objs;
}

QVector<QString> RamAbstractObjectModel::toVector() const
{
    return m_objectUuids.toVector();
}

QStringList RamAbstractObjectModel::toStringList() const
{
    return m_objectUuids;
}

bool RamAbstractObjectModel::contains(QString uuid) const
{
    return m_objectUuids.contains(uuid);
}

RamAbstractObject::ObjectType RamAbstractObjectModel::type() const
{
    return RamObject::objectTypeFromName( m_table );
}

int RamAbstractObjectModel::uuidRow(QString uuid) const
{
    return m_objectUuids.indexOf(uuid);
}

void RamAbstractObjectModel::insertObject(int row, QString uuid, QString data)
{
    // Add to list
    if (!m_objectUuids.contains(uuid)) m_objectUuids.insert(row, uuid);
    // Insert in lookup tables
    QHash<QString, QMultiHash<QString, QString>>::iterator i = m_lookUpTables.begin();
    while (i != m_lookUpTables.end())
    {
        QString lookUpValue = getLookUpValue(i.key(), data);
        i.value().insert(lookUpValue, uuid);
        i++;
    }
}

void RamAbstractObjectModel::removeObject(QString uuid)
{
    // Remove from uuid list
    m_objectUuids.removeAll(uuid);
    // Remove from lookup table
    removeObjectFromLookUp(uuid);
}

void RamAbstractObjectModel::updateObject(QString uuid, QString data)
{
    QJsonDocument doc = QJsonDocument::fromJson( data.toUtf8() );
    QJsonObject dataObj = doc.object();

    // Update lookup tables
    QHash<QString, QMultiHash<QString, QString>>::iterator i = m_lookUpTables.begin();
    while (i != m_lookUpTables.end())
    {
        QString lookUpValue = getLookUpValue(i.key(), data);

        QStringList lookup = i.value().values(lookUpValue);
        if (!lookup.contains(uuid))
        {
            removeObjectFromLookUp(uuid);
            i.value().insert(lookUpValue, uuid);
        }
        i++;
    }
}

void RamAbstractObjectModel::moveObjects(int from, int count, int to)
{
    int sourceEnd = from + count-1;

    for (int i = 0; i < count ; i++)
    {
        if (to < from) {
            m_objectUuids.move(sourceEnd, to);
        }
        else {
            m_objectUuids.move(from, to);
        }
    }
}

QString RamAbstractObjectModel::getLookUpValue(QString key, QString data)
{
    QJsonDocument doc = QJsonDocument::fromJson( data.toUtf8() );
    QJsonObject obj = doc.object();
    return obj.value(key).toString("default");
}

void RamAbstractObjectModel::removeObjectFromLookUp(QString uuid)
{
    QHash<QString, QMultiHash<QString, QString>>::iterator i = m_lookUpTables.begin();
    while (i != m_lookUpTables.end())
    {
        // Remove from lookup table
        QMutableHashIterator<QString, QString> it(i.value());
        while (it.hasNext())
        {
            it.next();
            if (it.value() == uuid) it.remove();
        }
        i++;
    }
}
