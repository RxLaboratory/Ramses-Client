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
    ~RamStep();

    bool isTemplate() const;

    Type type() const;
    void setType(const Type &type);
    void setType(QString type);

    void update();

private:
    bool _template;
    Type _type;
};

#endif // RAMSTEP_H
