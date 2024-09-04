#ifndef RAMSTEP_H
#define RAMSTEP_H

#include <QDesktopServices>

#include "ramtemplatestep.h"
#include "ramstatus.h"

class RamAssetGroup;
class RamProject;
class RamApplication;
class RamFileType;
class RamUser;
class RamWorkingFolder;
class RamObjectList;
class RamState;

class RamStep : public RamTemplateStep
{
    Q_OBJECT
public:

    struct StateCount {
        RamState *state;
        int count;
    };

    static RamStep *get(QString uuid, bool includeRemoved = false);
    static RamStep *c(RamObject *o);
    static RamStep *createFromTemplate(RamTemplateStep *tempStep, RamProject *project);

    // THE JSON DATA KEYS

    static const QString KEY_EstimationMultiplyGroup;

    // METHODS //

    RamStep(QString shortName, QString name, RamProject *project);

    RamProject *project() const;

    RamAssetGroup *estimationMultiplyGroup() const;
    void setEstimationMultiplyGroup(RamObject *newEstimationMultiplyGroup);

    float estimation(RamUser *user = nullptr) ; //days
    float latenessRatio(RamUser *user = nullptr) ;
    int completionRatio(RamUser *user = nullptr);
    float assignedDays(RamUser *user = nullptr) ;
    float futureDays(RamUser *user = nullptr);
    float unassignedDays() ;
    float missingDays(RamUser *user = nullptr) ;
    float daysSpent(RamUser *user = nullptr);
    float neededDays(RamUser *user = nullptr) ;

    QVector<StateCount> stateCount(RamUser *user = nullptr);
    QMap<RamStatus::Difficulty,int> difficultyCount(RamUser *user = nullptr);

    /**
     * @brief stats
     * @return a list of number of days <estimation, completed, scheduled, scheduled in the future>
     */
    QVector<float> stats(RamUser *user);

    void openFile(QString filePath) const;
    QSet<RamWorkingFolder> templateWorkingFolders() const;

    QSet<RamObject *> inputFileTypes();
    QSet<RamObject *> outputFileTypes();

    virtual QString fileName() const override;

public slots:
    virtual void edit(bool show = true) override;

protected:
    static QHash<QString, RamStep*> m_existingObjects;
    RamStep(QString uuid);
    virtual QString folderPath() const override;

    static QFrame *ui_editWidget;

private:
    void construct();
};

#endif // RAMSTEP_H
