#ifndef RAMSTEP_H
#define RAMSTEP_H

#include "ramobject.h"
#include "ramuser.h"

class RamStep : public RamObject
{
    Q_OBJECT
public:
    enum Type{ PreProduction, AssetProduction, ShotProduction, PostProduction };
    Q_ENUM(Type)

    explicit RamStep(QString shortName, QString name = "", bool tplt = true,  QString uuid = "", QObject *parent = nullptr);
    explicit RamStep(QString shortName, QString name, QString projectUuid,  QString uuid = "", QObject *parent = nullptr);
    ~RamStep();

    bool isTemplate() const;
    RamStep *createFromTemplate(QString projectUuid);

    Type type() const;
    void setType(const Type &type);
    void setType(QString type);

    QString projectUuid() const;
    void setProjectUuid(const QString &projectUuid);

    int order() const;
    void setOrder(int order);

    QList<RamUser *> users() const;
    void clearUsers();
    void assignUser(RamUser *user);
    void removeUser(RamUser *user);
    void removeUser(QString uuid);

    void update();

signals:
    void newUser(RamUser *);
    void userRemoved(QString);

private slots:
    void userDestroyed(QObject *o);

private:
    bool _template;
    Type _type;
    QString _projectUuid;
    int _order;
    QList<RamUser*> _users;
};

#endif // RAMSTEP_H
