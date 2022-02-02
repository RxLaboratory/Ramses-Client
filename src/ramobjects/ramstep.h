#ifndef RAMSTEP_H
#define RAMSTEP_H

#include <QDesktopServices>

#include "ramobject.h"
#include "ramuser.h"
#include "ramapplication.h"
#include "ramworkingfolder.h"
#include "data-models/ramobjectlist.h"

class RamAssetGroup;
class RamProject;

class RamStep : public RamObject
{
    Q_OBJECT
public:
    enum Type{ PreProduction, AssetProduction, ShotProduction, PostProduction, All };
    Q_ENUM(Type)

    enum EstimationMethod { EstimatePerShot = 0,
                            EstimatePerSecond = 1 };
    Q_ENUM(EstimationMethod)

    // Template (no project set)
    explicit RamStep(QString shortName, QString name = "", QString uuid = "");
    // Actual step
    explicit RamStep(QString shortName, QString name, RamProject *project,  QString uuid = "");
    ~RamStep();
    void init();

    /**
     * @brief freezeEstimations stops automatic update of the estimations.
     * Use this to improve performance when loading a bunch of data.
     * @param freeze
     */
    void freezeEstimations(bool freeze = true, bool reCompute = true);

    bool isTemplate() const;
    RamStep *createFromTemplate(RamProject *project);
    RamStep *createFromTemplate(QString projectUuid);

    Type type() const;
    void setType(const Type &type);
    void setType(QString type);

    RamProject *project() const;
    void setProject( RamProject *project );

    RamObjectList *applications() const;

    void openFile(QString filePath) const;
    QList<RamWorkingFolder> templateWorkingFolders() const;

    QList<RamObject *> inputFileTypes();
    QList<RamObject *> outputFileTypes();

    static RamStep *step(QString uuid);

    const QColor &color() const;
    void setColor(const QColor &newColor);

    const EstimationMethod &estimationMethod() const;
    void setEstimationMethod(const EstimationMethod &newEstimationMethod);

    float estimationVeryEasy() const;
    void setEstimationVeryEasy(float newEstimationVeryEasy);

    float estimationEasy() const;
    void setEstimationEasy(float newEstimationEasy);

    float estimationMedium() const;
    void setEstimationMedium(float newEstimationMedium);

    float estimationHard() const;
    void setEstimationHard(float newEstimationHard);

    float estimationVeryHard() const;
    void setEstimationVeryHard(float newEstimationVeryHard);

    RamAssetGroup *estimationMultiplyGroup() const;
    void setEstimationMultiplyGroup(RamAssetGroup *newEstimationMultiplyGroup);

    qint64 timeSpent() const; //seconds
    float estimation() const; //days
    float completionRatio() const;
    float latenessRatio() const;
    float assignedDays() const;
    float unassignedDays() const;
    float futureAssignedDays() const;

signals:
    void estimationComputed(RamStep*);

public slots:
    void update() override;
    bool move(int index) override;
    virtual void edit(bool show = true) override;
    virtual void removeFromDB() override;

    void computeEstimation();
    void countAssignedDays();


protected:
    virtual QString folderPath() const override;

private slots:
    void applicationAssigned(const QModelIndex &parent, int first, int last);
    void applicationUnassigned(const QModelIndex &parent, int first, int last);

private:
    bool m_template;
    Type m_type;
    QColor m_color;
    RamProject *m_project;
    RamObjectList *m_applications;

    /**
     * @brief When true, estimations won't be computed.
     * This should be set to true when loading a bunch of data, to improve performance
     * and reset to false afterwards.
     */
    bool m_freezeEstimations = false;

    EstimationMethod m_estimationMethod = EstimatePerShot;
    float m_estimationVeryEasy = 0.2;
    float m_estimationEasy = 0.5;
    float m_estimationMedium = 1.0;
    float m_estimationHard = 2.0;
    float m_estimationVeryHard = 3.0;
    RamAssetGroup *m_estimationMultiplyGroup = nullptr;
    bool m_estimationChanged = false;


    qint64 m_timeSpent = 0;
    float m_estimation = 0;
    float m_completionRatio = 0;
    float m_latenessRatio = 0;
    int m_assignedHalfDays = 0;
    int m_assignedFutureHalfDays = 0;
    float m_missingDays = 0;
};

#endif // RAMSTEP_H
