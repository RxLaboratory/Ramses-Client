#ifndef RAMASSETGROUP_H
#define RAMASSETGROUP_H

#include "ramasset.h"
#include "ramobjectlist.h"

class RamProject;

class RamAssetGroup : public RamObjectList
{
    Q_OBJECT
public:
    explicit RamAssetGroup(QString shortName, QString name = "",  QString uuid = "", QObject *parent = nullptr);
    explicit RamAssetGroup(QString shortName, RamProject *project, QString name,  QString uuid = "", QObject *parent = nullptr);
    ~RamAssetGroup();

    bool isTemplate() const;
    RamAssetGroup *createFromTemplate(QString projectUuid);

    RamProject *project() const;

    // Assets
    void append(RamAsset *asset);
    void createAsset(QString shortName = "NEW", QString name = "Asset");

    static RamAssetGroup *assetGroup(QString uuid);

public slots:
    void update() Q_DECL_OVERRIDE;
    void remove() Q_DECL_OVERRIDE;

private:
    bool _template;

    RamProject *m_project = nullptr;
};

#endif // RAMASSETGROUP_H
