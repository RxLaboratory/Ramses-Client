#ifndef RAMSTEP_H
#define RAMSTEP_H

#include <QDesktopServices>

#include "ramobject.h"

class RamAssetGroup;
class RamProject;
class RamApplication;
class RamFileType;
class RamUser;
class RamWorkingFolder;
template<typename RO> class RamObjectList;

class RamStep : public RamObject
{
    Q_OBJECT
public:

    // ENUMS //

    enum Type{ PreProduction,
               AssetProduction,
               ShotProduction,
               PostProduction,
               All };
    Q_ENUM(Type)

    enum EstimationMethod { EstimatePerShot = 0,
                            EstimatePerSecond = 1 };
    Q_ENUM(EstimationMethod)

    // STATIC METHODS //

    /**
     * @brief stepTypeName gets the name of a type, as used in the Database and API classes
     * @param type
     * @return
     */
    static const QString stepTypeName(Type type);
    static Type stepTypeFromName(QString typeName);

    static RamStep *getObject(QString uuid, bool constructNew = false);

    // METHODS //

    // Template (no project set)
    explicit RamStep(QString shortName, QString name);
    // Actual step
    explicit RamStep(QString shortName, QString name, RamProject *project);

    bool isTemplate() const;
    RamStep *createFromTemplate(RamProject *project);
    RamStep *createFromTemplate(QString projectUuid);

    RamProject *project() const;
    RamObjectList<RamApplication*> *applications() const;

    Type type() const;
    void setType(const Type &type);
    void setType(QString type);

    QColor color() const;
    void setColor(const QColor &newColor);

    QString publishSettings() const;
    void setPublishSettings(const QString &newPublishSettings);

    EstimationMethod estimationMethod() const;
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
    float missingDays() const;
    float daysSpent() const;
    float neededDays() const;
    /**
     * @brief stats
     * @return a list of number of days <estimation, completed, scheduled, scheduled in the future>
     */
    QList<float> stats(RamUser *user);

    /**
     * @brief freezeEstimations stops automatic update of the estimations.
     * Use this to improve performance when loading a bunch of data.
     * @param freeze Whether to freeze the estimations
     * @param reCompute When false, the estimation will not be recomputed on unfreeze
     */
    void freezeEstimations(bool freeze = true, bool reCompute = true);

    void openFile(QString filePath) const;
    QList<RamWorkingFolder> templateWorkingFolders() const;

    QList<RamFileType *> inputFileTypes();
    QList<RamFileType *> outputFileTypes();

signals:
    void estimationComputed(RamStep*);

public slots:
    virtual void edit(bool show = true) override;

    void computeEstimation();
    void countAssignedDays();

protected:

    // STATIC //

    /**
     * @brief m_stepTypeMeta is used to return information from the type,
     * like the corresponding string
     */
    static const QMetaEnum m_stepTypeMeta;

    // METHODS //

    RamStep(QString uuid);

    virtual QString folderPath() const override;

private:
    void construct();


    RamProject *m_project;
    RamObjectList<RamApplication*> *m_applications;

    /**
     * @brief When true, estimations won't be computed.
     * This should be set to true when loading a bunch of data, to improve performance
     * and reset to false afterwards.
     */
    bool m_freezeEstimations = false;

    /**
     * @brief m_estimationChanged is set to true if the estimation needs to be recomputed
     */
    bool m_estimationChanged = false;

    // Estimation cache
    qint64 m_timeSpent = 0;
    float m_estimation = 0;
    float m_completionRatio = 0;
    float m_latenessRatio = 0;
    int m_scheduledHalfDays = 0;
    int m_scheduledFutureHalfDays = 0;
    float m_missingDays = 0;
};

#endif // RAMSTEP_H
