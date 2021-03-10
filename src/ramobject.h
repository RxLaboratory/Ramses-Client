#ifndef RAMOBJECT_H
#define RAMOBJECT_H

#include "dbinterface.h"
#include "ramuuid.h"

#include <QObject>

class RamObject : public QObject
{
    Q_OBJECT
public:
    explicit RamObject(QString shortName, QString name = "", QString uuid = "", QObject *parent = nullptr);

    QString shortName() const;
    void setShortName(const QString &shortName);

    QString name() const;
    void setName(const QString &name);

    QString uuid() const;
    void setUuid(const QString &uuid);

    void remove();

signals:
    void changed(RamObject *);
    void removed(RamObject *);

protected:
    DBInterface *_dbi;
    QString _shortName;
    QString _name;
    QString _uuid;
    bool _removing;
};

#endif // RAMOBJECT_H
