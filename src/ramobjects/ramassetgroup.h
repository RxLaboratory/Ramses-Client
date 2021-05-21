#ifndef RAMASSETGROUP_H
#define RAMASSETGROUP_H

#include "ramasset.h"
#include "ramobjectlist.h"

class RamAssetGroup : public RamObjectList
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

    // Assets
    RamAsset *asset(QString uuid) const;
    void append(RamAsset *asset);
    void createAsset(QString shortName = "NEW", QString name = "Asset");

    void update() Q_DECL_OVERRIDE;

private:
    bool _template;
    QString _projectUuid;
};

#endif // RAMASSETGROUP_H
