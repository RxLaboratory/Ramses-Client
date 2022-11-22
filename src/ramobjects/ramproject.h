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
class DBTableModel;
class RamStatusTableModel;
class RamStatus;
class RamAbstractItem;
class RamState;

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
    DBTableModel *steps() const;
    DBTableModel *shotSteps() const;
    DBTableModel *assetSteps() const;
    // Asset Groups
    DBTableModel *assetGroups() const;
    // Sequences
    DBTableModel *sequences() const;
    // Shots
    DBTableModel *shots() const;
    // Assets
    DBTableModel *assets() const;
    // Pipeline
    RamObjectModel *pipeline() const;
    DBTableModel *pipeFiles() const;
    // Users
    RamObjectModel *users() const;
    // Schedule comments
    DBTableModel *scheduleComments() const;
    // Status
    RamStatusTableModel *assetStatus() const;
    RamStatusTableModel *shotStatus() const;
    RamStatus *status(RamAbstractItem *item, RamStep *step) const;
    QSet<RamStatus*> itemStatus(RamAbstractItem *item) const;
    QSet<RamStatus*> stepStatus(RamStep *step) const;
    RamState *state(RamAbstractItem *item, RamStep *step) const;
    bool hasState(RamObject *stateObj, RamAbstractItem *item, RamStep *step) const;
    bool hasState(RamObject *stateObj, RamStep *step) const;
    bool hasState(RamObject *stateObj, RamAbstractItem *item) const;
    RamUser *assignedUser(RamAbstractItem *item, RamStep *step) const;
    bool isUserAssigned(RamObject *userObj, RamAbstractItem *item, RamStep *step) const;
    bool isUserAssigned(RamObject *userObj, RamAbstractItem *item) const;
    bool isUserAssigned(RamObject *userObj, RamStep *step) const;
    bool isUnassigned(RamAbstractItem *item, RamStep *step) const;
    bool isUnassigned(RamStep *step) const;
    bool isUnassigned(RamAbstractItem *item) const;

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
    QVector<float> stats(RamUser *user);

    void setFolderPath(const QString &newFolderPath);
    void resetDbFolderPath();
    QString defaultPath() const;
    bool pathIsDefault(QString p) const;

    QString dbFolderPath() const;
    void setDbFolderPath(const QString &newDbFolderPath);

    virtual QString details() const override;

    virtual QStringList filterListUuids() const override;

signals:
    void completionRatioChanged(int);
    void latenessRatioChanged(float);
    void timeSpentChanged(qint64);
    void estimationChanged(float);
    void estimationComputed(RamProject*);

public slots:
    void updatePath();
    virtual void edit(bool show = true) override;
    void computeEstimation(bool recompute = false);

protected:
    static QHash<QString, RamProject*> m_existingObjects;
    RamProject(QString uuid);
    virtual QString folderPath() const override;

    static QFrame *ui_editWidget;

private:
    void construct();
    void getCreateLists();

    // LISTS
    DBTableModel *m_steps;
    DBTableModel *m_shotSteps;
    DBTableModel *m_assetSteps;
    DBTableModel *m_sequences;
    DBTableModel *m_assetGroups;
    DBTableModel *m_assets;
    DBTableModel *m_shots;
    RamObjectModel *m_pipeline;
    DBTableModel *m_pipeFiles;
    RamObjectModel *m_users;
    DBTableModel *m_scheduleComments;
    RamStatusTableModel *m_assetStatusTable;
    RamStatusTableModel *m_shotStatusTable;

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
