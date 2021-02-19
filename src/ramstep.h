#ifndef RAMSTEP_H
#define RAMSTEP_H

#include "ramobject.h"

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

    void update();

private:
    bool _template;
    Type _type;
    QString _projectUuid;
};

#endif // RAMSTEP_H
