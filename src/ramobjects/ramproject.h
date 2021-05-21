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
    QList<RamStep *> steps() const;
    RamStep *step(QString uuid) const;
    void addStep(RamStep *step);
    void assignStep(RamStep *templateStep);
    void createStep(QString shortName = "NEW", QString name = "Step");
    void removeStep(QString uuid);
    void removeStep(RamObject *step);
    void sortSteps();

    // Asset Groups
    QList<RamAssetGroup *> assetGroups() const;
    RamAssetGroup *assetGroup(QString uuid);
    void addAssetGroup(RamAssetGroup *assetGroup);
    void assignAssetGroup(RamAssetGroup *templateAssetGroup);
    void createAssetGroup(QString shortName = "NEW", QString name = "Asset Group");
    void removeAssetGroup(QString uuid);
    void removeAssetGroup(RamObject *assetGroup);
    void sortAssetGroups();

    // Assets
    QList<RamAsset*> assets();
    RamAsset *asset(QString uuid) const;
    void moveAssetToGroup(RamAsset *asset, QString groupUuid);
    void moveAssetToGroup(RamAsset *asset, RamAssetGroup *group);
    void removeAsset(QString uuid);

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

    void update();

signals:
    void newStep(RamStep *);
    void stepRemoved(RamStep *);
    void newAssetGroup(RamAssetGroup *);
    void assetGroupRemoved(QString uuid);
    void newPipe(RamPipe *);
    void pipeRemoved(RamPipe *p);

private slots:
    void stepRemoved(RamObject *o);
    void assetGroupRemoved(RamObject *o);

private:
    QString _folderPath;
    qreal _framerate = 24;
    int _width = 1920;
    int _height = 1080;
    qreal _aspectRatio = 1.78;
    QList<RamStep *> _steps;
    QList<RamAssetGroup *> _assetGroups;
    RamObjectUberList *_sequences;
    RamObjectList *_pipeline;
};

#endif // RAMPROJECT_H
