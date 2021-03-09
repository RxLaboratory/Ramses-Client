#ifndef RAMASSETGROUP_H
#define RAMASSETGROUP_H

#include "ramobject.h"

class RamAssetGroup : public RamObject
{
    Q_OBJECT
public:
    explicit RamAssetGroup(QString shortName, QString name = "", bool tplt = true,  QString uuid = "", QObject *parent = nullptr);
    explicit RamAssetGroup(QString shortName, QString name, QString projectUuid,  QString uuid = "", QObject *parent = nullptr);
    ~RamAssetGroup();

    bool isTemplate() const;
    RamAssetGroup *createFromTemplate(QString projectUuid);

    QString projectUuid() const;
    void setProjectUuid(const QString &projectUuid);

    void update();

private:
    bool _template;
    QString _projectUuid;
};

#endif // RAMASSETGROUP_H
