#ifndef RAMLOADER_H
#define RAMLOADER_H

#include <QThread>

#include "processmanager.h"
#include "ramses.h"
#include "ramscheduleentry.h"
#include "ramschedulecomment.h"
#include "dbisuspender.h"

class RamLoader : public QThread
{
    Q_OBJECT
public:
    static RamLoader *instance();

    void run() override;

public slots:
    void newData(QJsonObject data);

signals:
    void refresh();
    void ready();
    void projectReady(QString);
    void loggedIn();

protected:
    static RamLoader *_instance;

private:
    RamLoader(QObject *parent = nullptr);

    void login(QJsonObject user);
    //users
    void gotUsers(QJsonArray users);
    QString gotUser(QJsonObject newU);
    //projects
    void gotProjects(QJsonArray projects, bool init);
    QString gotProject(QJsonObject newP, bool init);
    //template steps
    void gotTemplateSteps(QJsonArray steps);
    QString gotTemplateStep(QJsonObject newS);
    //template asset groups
    void gotTemplateAssetGroups(QJsonArray assetGroups);
    QString gotTemplateAssetGroup(QJsonObject newAG);
    //states
    void gotStates(QJsonArray states);
    QString gotState(QJsonObject newS);
    //file types
    void gotFileTypes(QJsonArray fileTypes);
    QString gotFileType(QJsonObject newFt);
    //applications
    void gotApplications(QJsonArray applications);
    QString gotApplication(QJsonObject newA);
    // steps
    void gotSteps(QJsonArray steps, RamProject *project, bool freezeEstimations = false);
    QString gotStep(QJsonObject newS, RamProject *project, bool freezeEstimations = false);
    // asset groups
    void gotAssetGroups(QJsonArray assetGroups, RamProject *project);
    QString gotAssetGroup(QJsonObject newAG, RamProject *project);
    // assets
    void gotAssets(QJsonArray assets, RamProject *project);
    QString gotAsset(QJsonObject newA, RamProject *project);
    // sequences
    void gotSequences(QJsonArray sequences, RamProject *project);
    QString gotSequence(QJsonObject newS, RamProject *project);
    // shots
    void gotShots(QJsonArray shots, RamProject *project);
    QString gotShot(QJsonObject newS, RamProject *project);
    // status
    void gotStatusHistory(QJsonArray statusHistory, RamItem *item);
    QString gotStatus(QJsonObject newS, RamItem *item);
    // pipes
    void gotPipes(QJsonArray pipes, RamProject *project);
    QString gotPipe(QJsonObject newP, RamProject *project);
    // pipefiles
    void gotPipeFiles(QJsonArray pipeFiles, RamProject *project);
    QString gotPipeFile(QJsonObject newPF, RamProject *project);
    // schedule
    void gotSechedule(QJsonArray schedule);
    QString gotScheduleEntry(QJsonObject newSE);
    void gotScheduleComments(QJsonArray comments);
    QString gotScheduleComment(QJsonObject newComment);

    QJsonObject m_data;

    QString m_currentUserShortName;

    ProcessManager *m_pm;
    Ramses *m_ram;


};

#endif // RAMLOADER_H
