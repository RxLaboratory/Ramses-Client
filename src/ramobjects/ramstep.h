#ifndef RAMSTEP_H
#define RAMSTEP_H

#include <QDesktopServices>

#include "ramtemplatestep.h"

class RamAssetGroup;
class RamProject;
class RamApplication;
class RamFileType;
class RamUser;
class RamWorkingFolder;
class RamObjectList;

class RamStep : public RamTemplateStep
{
    Q_OBJECT
public:
    static RamStep *get(QString uuid, bool includeRemoved = false);
    static RamStep *c(RamObject *o);
    static RamStep *createFromTemplate(RamTemplateStep *tempStep, RamProject *project);

    // METHODS //

    RamStep(QString shortName, QString name, RamProject *project);

    RamProject *project() const;

    RamAssetGroup *estimationMultiplyGroup() const;
    void setEstimationMultiplyGroup(RamObject *newEstimationMultiplyGroup);

    float estimation() ; //days
    float latenessRatio() ;
    int completionRatio();
    float assignedDays() ;
    float unassignedDays() ;
    float missingDays() ;
    float daysSpent() ;
    float neededDays() ;

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

signals:
    void estimationComputed(RamStep*);

public slots:
    virtual void edit(bool show = true) override;

protected:
    static QHash<QString, RamStep*> m_existingObjects;
    RamStep(QString uuid);
    virtual QString folderPath() const override;

    static QFrame *ui_editWidget;

private:
    void construct();

    void computeEstimation();
    void countAssignedDays();

    // Estimation cache
    float m_estimation = 0;
    int m_completionRatio = 0;
    QElapsedTimer m_cacheEstimationTimer;
    // Schedule cache
    int m_scheduledHalfDays = 0;
    int m_scheduledFutureHalfDays = 0;
    QElapsedTimer m_cacheScheduleTimer;
};

#endif // RAMSTEP_H
