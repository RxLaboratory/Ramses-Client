#ifndef RAMOBJECT_H
#define RAMOBJECT_H

#include <QObject>
#include <QMap>

#include "dbinterface.h"
#include "ramuuid.h"

class RamObject : public QObject
{
    Q_OBJECT
public:
    enum ObjectType { Application,
                    Asset,
                    AssetGroup,
                    FileType,
                    Generic,
                    Item,
                    Pipe,
                    PipeFile,
                    Project,
                    Sequence,
                    Shot,
                    State,
                    Status,
                    Step,
                    User};
    Q_ENUM( ObjectType )

    explicit RamObject(QObject *parent = nullptr);
    explicit RamObject(QString uuid, QObject *parent = nullptr);
    explicit RamObject(QString shortName, QString name, QString uuid = "", QObject *parent = nullptr);

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

    bool is(const RamObject *other);

    static RamObject *obj(QString uuid);

public slots:
    virtual void update();
    virtual void remove();

signals:
    void changed(RamObject *);
    void orderChanged(RamObject *, int p, int n);
    void removed(RamObject *);

protected:
    DBInterface *_dbi;
    QString _shortName;
    QString _name;
    // Check if changed to limit number of signals
    bool _dirty;

    QString _uuid;
    int _order = -1;
    bool _orderChanged = false;
    bool _removing;

    static QMap<QString, RamObject*> m_existingObjects;

private:
    RamObject::ObjectType _objectType = Generic;

};

#endif // RAMOBJECT_H
