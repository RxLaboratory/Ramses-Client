#ifndef RAMLOADER_H
#define RAMLOADER_H

#include <QThread>

#include "processmanager.h"
#include "ramses.h"

class RamLoader : public QThread
{
    Q_OBJECT
public:
    static RamLoader *instance();

    void run() Q_DECL_OVERRIDE;

public slots:
    void newData(QJsonObject data);

signals:
    void refresh();
    void ready();
    void projectReady(QString);

protected:
    static RamLoader *_instance;

private:
    RamLoader(QObject *parent = nullptr);

    void login(QJsonObject user);
    //users
    void gotUsers(QJsonArray users);
    //projects
    void gotProjects(QJsonArray projects);
    QString gotProject(QJsonObject newP);
    //template steps
    void gotTemplateSteps(QJsonArray steps);
    //template asset groups
    void gotTemplateAssetGroups(QJsonArray assetGroups);
    //states
    void gotStates(QJsonArray states);
    //file types
    void gotFileTypes(QJsonArray fileTypes);
    //applications
    void gotApplications(QJsonArray applications);
    // steps
    void gotSteps(QJsonArray steps, RamProject *project);
    QString gotStep(QJsonObject newS, RamProject *project);
    // asset groups
    void gotAssetGroups(QJsonArray assetGroups, RamProject *project);
    QString gotAssetGroup(QJsonObject newAG, RamProject *project);
    // assets
    void gotAssets(QJsonArray assets, RamAssetGroup *assetGroup, RamProject *project);
    QString gotAsset(QJsonObject newA, RamAssetGroup *assetGroup, RamProject *project);
    // sequences
    void gotSequences(QJsonArray sequences, RamProject *project);
    QString gotSequence(QJsonObject newS, RamProject *project);
    // shots
    void gotShots(QJsonArray shots, RamSequence *sequence, RamProject *project);
    QString gotShot(QJsonObject newS, RamSequence *sequence, RamProject *project);
    // status
    void gotStatusHistory(QJsonArray statusHistory, RamItem *item, RamProject *project);
    QString gotStatus(QJsonObject newS, RamItem *item, RamProject *project);
    // pipes
    void gotPipes(QJsonArray pipes, RamProject *project);
    QString gotPipe(QJsonObject newP, RamProject *project);

    QJsonObject m_data;

    QString m_currentUserShortName;

    ProcessManager *m_pm;
    Ramses *m_ram;
};

#endif // RAMLOADER_H
