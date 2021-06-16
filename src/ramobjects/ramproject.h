#ifndef RAMPROJECT_H
#define RAMPROJECT_H

#include "ramobject.h"
#include "ramstep.h"
#include "ramassetgroup.h"
#include "ramsequence.h"
#include "rampipe.h"
#include "dbisuspender.h"
#include "ramobjectlist.h"
#include "ramobjectuberlist.h"
#include "rampipefile.h"

class RamProject : public RamObject
{
    Q_OBJECT
public:
    RamProject(QString shortName, QString name = "", QString uuid = "", QObject *parent = nullptr);
    ~RamProject();

    QString folderPath() const;
    void setFolderPath(const QString &folderPath);

    qreal framerate() const;
    void setFramerate(const qreal &framerate);

    int width() const;
    void setWidth(const int width, const qreal &pixelAspect = 1);

    int height() const;
    void setHeight(const int height, const qreal &pixelAspect = 1);

    qreal aspectRatio() const;
    void updateAspectRatio(const qreal &pixelAspect = 1);
    void setAspectRatio(const qreal &aspectRatio);

    // Steps
    RamObjectList *steps() const;
    void assignStep(RamStep *templateStep);
    void createStep(QString shortName = "NEW", QString name = "Step");

    // Asset Groups
    RamObjectUberList *assetGroups() const;
    RamAssetGroup *assetGroup(QString uuid);
    void createAssetGroup(QString shortName = "NEW", QString name = "Asset Group");

    // Assets
    void moveAssetToGroup(RamAsset *asset, QString groupUuid);
    void moveAssetToGroup(RamAsset *asset, RamAssetGroup *group);

    // Sequences
    RamObjectUberList *sequences() const;
    RamSequence *sequence(QString uuid);
    void createSequence(QString shortName = "NEW", QString name = "Sequence");

    // Shots
    void moveShotToSequence(RamShot *shot, QString sequenceUuid);
    void moveShotToSequence(RamShot *shot, RamSequence *sequence);

    // Pipeline
    RamObjectList *pipeline();
    RamPipe *pipe(RamStep *outputStep, RamStep *inputStep);
    RamPipe *createPipe(RamStep *output, RamStep *input);
    RamObjectList *pipeFiles();
    RamPipeFile *createPipeFile(QString shortName = "NEW");

    void update();

private:
    QString _folderPath;
    qreal _framerate = 24;
    int _width = 1920;
    int _height = 1080;
    qreal _aspectRatio = 1.78;
    RamObjectList *_steps;
    RamObjectUberList *_sequences;
    RamObjectUberList *_assetGroups;
    RamObjectList *_pipeline;
    RamObjectList *_pipeFiles;
};

#endif // RAMPROJECT_H
