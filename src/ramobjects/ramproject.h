#ifndef RAMPROJECT_H
#define RAMPROJECT_H

#include "ramobject.h"

class RamSequence;
class RamPipe;
class RamPipeFile;
class RamScheduleComment;
class RamStep;
class RamAssetGroup;
class RamItemTable;
class RamUser;
class RamObjectList;

class RamProject : public RamObject
{
    Q_OBJECT
public:

    // STATIC METHODS //

    static RamProject *get(QString uuid);
    static RamProject *c(RamObject *o);

    // METHODS //

    RamProject(QString shortName, QString name);

    // Steps
    RamObjectList *steps() const;
    // Asset Groups
    RamObjectList *assetGroups() const;
    // Sequences
    RamObjectList *sequences() const;
    // Shots
    RamItemTable *shots() const;
    // Assets
    RamItemTable *assets() const;
    // Pipeline
    RamObjectList *pipeline() const;
    RamObjectList *pipeFiles() const;
    // Users
    RamObjectList *users() const;
    // Schedule comments
    RamObjectList *scheduleComments() const;

    qreal framerate() const;
    void setFramerate(const qreal &newFramerate);

    int width() const;
    void setWidth(const int width, const qreal &pixelAspect = 1);

    int height() const;
    void setHeight(const int height, const qreal &pixelAspect = 1);

    qreal aspectRatio() const;
    void updateAspectRatio(const qreal &pixelAspect = 1);
    void setAspectRatio(const qreal &aspectRatio);

    QDate deadline() const;
    void setDeadline(const QDate &newDeadline);

    double duration() const;

    RamPipe *pipe(RamStep *outputStep, RamStep *inputStep);

    /**
     * @brief freezeEstimations stops automatic update of the estimations.
     * Use this to improve performance when loading a bunch of data.
     * @param freeze
     */
    void freezeEstimations(bool freeze = true, bool reCompute = true);
    // Production Tracking
    qint64 timeSpent() const; //seconds
    float estimation() const; //days
    int completionRatio() const; //%
    float latenessRatio() const; //ratio
    float assignedDays() const; //days
    float unassignedDays() const; //days
    /**
     * @brief stats
     * @return a list of number of days <estimation, completed, assigned, future>
     */
    QList<float> stats(RamUser *user);

    void setFolderPath(const QString &newFolderPath);
    void resetDbFolderPath();
    QString defaultPath() const;
    bool pathIsDefault(QString p) const;

    QString dbFolderPath() const;
    void setDbFolderPath(const QString &newDbFolderPath);

    virtual QString details() const override;

signals:
    void completionRatioChanged(int);
    void latenessRatioChanged(float);
    void timeSpentChanged(qint64);
    void estimationChanged(float);
    void estimationComputed(RamProject*);

public slots:
    void updatePath();
    virtual void edit(bool show = true) override;
    void computeEstimation();

protected:
    static QMap<QString, RamProject*> m_existingObjects;
    RamProject(QString uuid);
    virtual QString folderPath() const override;

private slots:
    void listChanged(RamObjectList *list);

private:
    void construct();
    void getCreateLists();

    // LISTS
    RamObjectList *m_steps;
    RamObjectList *m_sequences;
    RamObjectList *m_assetGroups;
    RamItemTable *m_assets;
    RamItemTable *m_shots;
    RamObjectList *m_pipeline;
    RamObjectList *m_pipeFiles;
    RamObjectList *m_users;
    RamObjectList *m_scheduleComments;

    /**
     * @brief When true, estimations won't be computed.
     * This should be set to true when loading a bunch of data, to improve performance
     * and reset to false afterwards.
     */
    bool m_freezeEstimations = false;

    qint64 m_timeSpent = 0;
    float m_estimation = 0;
    int m_completionRatio = 0;
    float m_latenessRatio = 0;
    float m_missingDays = 0;
    float m_assignedDays = 0;
};

#endif // RAMPROJECT_H
