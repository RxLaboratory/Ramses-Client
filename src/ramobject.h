#ifndef RAMOBJECT_H
#define RAMOBJECT_H

#include "dbinterface.h"

#include <QObject>

class RamObject : public QObject
{
    Q_OBJECT
public:
    explicit RamObject(QString shortName, QString name = "", QString uuid = "", QString path = "", QObject *parent = nullptr);

    QString shortName() const;
    void setShortName(const QString &shortName);

    QString name() const;
    void setName(const QString &name);

    QString folderPath() const;
    void setFolderPath(const QString &folderPath);

    QString uuid() const;
    void setUuid(const QString &uuid);

signals:


protected:
    DBInterface *_dbi;
    QString _shortName;
    QString _name;
    QString _folderPath;
    QString _uuid;
};

#endif // RAMOBJECT_H
