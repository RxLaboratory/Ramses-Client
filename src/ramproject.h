#ifndef RAMPROJECT_H
#define RAMPROJECT_H

#include "ramobject.h"
#include "ramstep.h"
#include "ramassetgroup.h"
#include "ramsequence.h"
#include "dbisuspender.h"

class RamProject : public RamObject
{
    Q_OBJECT
public:
    RamProject(QString shortName, QString name = "", QString uuid = "", QObject *parent = nullptr);
    ~RamProject();

    QString folderPath() const;
    void setFolderPath(const QString &folderPath);

    // Steps
    QList<RamStep *> steps() const;
    RamStep *step(QString uuid) const;
    void addStep(RamStep *step);
    void assignStep(RamStep *templateStep);
    void createStep(QString shortName = "NEW", QString name = "Step");
    void removeStep(QString uuid);
    void removeStep(RamStep *step);
    void sortSteps();

    // Asset Groups
    QList<RamAssetGroup *> assetGroups() const;
    RamAssetGroup *assetGroup(QString uuid);
    void addAssetGroup(RamAssetGroup *assetGroup);
    void assignAssetGroup(RamAssetGroup *templateAssetGroup);
    void createAssetGroup(QString shortName = "NEW", QString name = "Asset Group");
    void removeAssetGroup(QString uuid);
    void removeAssetGroup(RamAssetGroup *assetGroup);
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
    void removeSequence(RamSequence *seq);
    void sortSequences();

    void update();

signals:
    void newStep(RamStep *);
    void stepRemoved(QString uuid);
    void newAssetGroup(RamAssetGroup *);
    void assetGroupRemoved(QString uuid);
    void newSequence(RamSequence *);
    void sequenceRemoved(RamSequence *);

private slots:
    void stepDestroyed(QObject *o);
    void assetGroupDestroyed(QObject *o);
    void sequenceDestroyed(QObject *o);

private:
    QString _folderPath;
    QList<RamStep *> _steps;
    QList<RamAssetGroup *> _assetGroups;
    QList<RamSequence *> _sequences;
};

#endif // RAMPROJECT_H
