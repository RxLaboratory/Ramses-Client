#ifndef RAMSTEP_H
#define RAMSTEP_H

#include "ramobject.h"
#include "ramuser.h"
#include "ramapplication.h"

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
    void unassignUser(RamUser *user);
    void unassignUser(QString uuid);

    QList<RamApplication *> applications() const;
    void clearApplications();
    void assignApplication(RamApplication *app);
    void unassignApplication(RamApplication *app);
    void unassignApplication(QString uuid);

    void update();

signals:
    void userAssigned(RamUser *);
    void userUnassigned(QString);
    void applicationAssigned(RamApplication *);
    void applicationUnassigned(QString);

private slots:
    void userDestroyed(QObject *o);
    void applicationRemoved(RamObject *o);

private:
    bool _template;
    Type _type;
    QString _projectUuid;
    int _order;
    QList<RamUser*> _users;
    QList<RamApplication*> _applications;
};

#endif // RAMSTEP_H
