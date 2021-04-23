#ifndef RAMOBJECT_H
#define RAMOBJECT_H

#include "dbinterface.h"
#include "ramuuid.h"

#include <QObject>

class RamObject : public QObject
{
    Q_OBJECT
public:
    enum ObjectType { Application,
                    Asset,
                    AssetGroup,
                    FileType,
                    Generic,
                    Pipe,
                    Project,
                    Sequence,
                    Shot,
                    State,
                    Status,
                    Step,
                    User};
    Q_ENUM( ObjectType )

    explicit RamObject(QString shortName, QString name = "", QString uuid = "", QObject *parent = nullptr);

    QString shortName() const;
    void setShortName(const QString &shortName);

    QString name() const;
    void setName(const QString &name);

    QString uuid() const;
    void setUuid(const QString &uuid);

    RamObject::ObjectType objectType() const;
    void setObjectType(ObjectType type);

    int order() const;
    void setOrder(int order);

public slots:
    virtual void update() = 0;
    void remove();

signals:
    void changed(RamObject *);
    void orderChanged(RamObject *, int p, int n);
    void removed(RamObject *);

protected:
    DBInterface *_dbi;
    QString _shortName;
    QString _name;
    QString _uuid;
    int _order = -1;
    bool _orderChanged = false;
    bool _removing;

private:
    RamObject::ObjectType _objectType = Generic;
};

#endif // RAMOBJECT_H
