#ifndef RAMPROJECT_H
#define RAMPROJECT_H

#include "ramobject.h"

class RamProject : public RamObject
{
    Q_OBJECT
public:
    RamProject(QString shortName, QString name = "", QString uuid = "", QString path = "", QObject *parent = nullptr);
    ~RamProject();

    void update();

};

#endif // RAMPROJECT_H
