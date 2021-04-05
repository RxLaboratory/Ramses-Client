#ifndef RAMPROJECT_H
#define RAMPROJECT_H

#include "ramobject.h"
#include "ramstep.h"
#include "ramassetgroup.h"
#include "ramsequence.h"
#include "rampipe.h"
#include "dbisuspender.h"

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
    RamAsset *asset(QString uuid);
    void removeAsset(QString uuid);

    // Sequences
    QList<RamSequence *> sequences() const;
    RamSequence *sequence(QString uuid);
    void addSequence(RamSequence *seq);
    void createSequence(QString shortName = "NEW", QString name = "Sequence");
    void removeSequence(QString uuid);
    void removeSequence(RamObject *seq);
    void sortSequences();

    // Shots
    RamShot *shot(QString uuid);
    void removeShot(QString uuid);

    // Pipeline
    QList<RamPipe *> pipeline();
    RamPipe *pipe(QString uuid);
    RamPipe *pipe(RamStep *o, RamStep *i);
    RamPipe* createPipe(RamStep *output, RamStep *input);
    void addPipe(RamPipe *pipe);
    void removePipe(QString uuid);
    void removePipe(RamPipe *pipe);

    void update();

signals:
    void newStep(RamStep *);
    void stepRemoved(QString uuid);
    void newAssetGroup(RamAssetGroup *);
    void assetGroupRemoved(QString uuid);
    void newSequence(RamSequence *);
    void sequenceRemoved(RamSequence *);
    void newPipe(RamPipe *);
    void pipeRemoved(RamPipe *p);

private slots:
    void stepRemoved(RamObject *o);
    void assetGroupRemoved(RamObject *o);
    void sequenceRemoved(RamObject *o);

private:
    QString _folderPath;
    qreal _framerate = 24;
    int _width = 1920;
    int _height = 1080;
    qreal _aspectRatio = 1.78;
    QList<RamStep *> _steps;
    QList<RamAssetGroup *> _assetGroups;
    QList<RamSequence *> _sequences;
    QList<RamPipe *> _pipeline;
};

#endif // RAMPROJECT_H
