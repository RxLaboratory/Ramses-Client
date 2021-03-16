#ifndef RAMFILETYPE_H
#define RAMFILETYPE_H

#include "ramobject.h"

class RamFileType : public RamObject
{
    Q_OBJECT
public:
    RamFileType(QString shortName, QString name = "", QStringList extensions = QStringList(), QString uuid = "", QObject *parent = nullptr);
    ~RamFileType();

    void update();

private:
    bool _removing = false;
    QStringList _extensions;
};

#endif // RAMFILETYPE_H
