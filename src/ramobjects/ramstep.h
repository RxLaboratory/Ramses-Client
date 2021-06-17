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
    void init();

    bool isTemplate() const;
    RamStep *createFromTemplate(QString projectUuid);

    Type type() const;
    void setType(const Type &type);
    void setType(QString type);

    QString projectUuid() const;
    void setProjectUuid(const QString &projectUuid);

    RamObjectList *users() const;
    RamObjectList *applications() const;

    QList<RamObject *> inputFileTypes();
    QList<RamObject *> outputFileTypes();

    void update();

    static RamStep *step(QString uuid);

private slots:
    void userAssigned(RamObject *u);
    void userUnassigned(RamObject *u);
    void applicationAssigned(RamObject *a);
    void applicationUnassigned(RamObject *a);

private:
    bool _template;
    Type _type;
    QString _projectUuid;
    RamObjectList *_users;
    RamObjectList *_applications;
};

#endif // RAMSTEP_H
