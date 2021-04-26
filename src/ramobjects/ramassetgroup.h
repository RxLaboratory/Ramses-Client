#ifndef RAMASSETGROUP_H
#define RAMASSETGROUP_H

#include "ramasset.h"

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

    // Assets
    QList<RamAsset *> assets() const;
    RamAsset *asset(QString uuid) const;
    void addAsset(RamAsset *asset);
    void createAsset(QString shortName = "NEW", QString name = "Asset");
    void removeAsset(QString uuid);
    void removeAsset(RamObject *asset);
    void sortAssets();

    void update();

public slots:
    void assetRemoved(RamObject *o);

signals:
    void newAsset(RamAsset *);
    void assetRemovedFromGroup(RamAsset*);

private:
    bool _template;
    QString _projectUuid;
    QList<RamAsset*> _assets;
};

#endif // RAMASSETGROUP_H
