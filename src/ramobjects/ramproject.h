#ifndef RAMPROJECT_H
#define RAMPROJECT_H

#include "ramobject.h"
#include "ramstep.h"
#include "ramassetgroup.h"
#include "ramsequence.h"
#include "rampipe.h"
#include "dbisuspender.h"
#include "data-models/ramitemtable.h"
#include "rampipefile.h"

class RamProject : public RamObject
{
    Q_OBJECT
public:   
    RamProject(QString shortName, QString name = "", QString uuid = "");
    ~RamProject();

    /**
     * @brief freezeEstimations stops automatic update of the estimations.
     * Use this to improve performance when loading a bunch of data.
     * @param freeze
     */
    void freezeEstimations(bool freeze = true, bool reCompute = true);

    void setFolderPath(const QString &folderPath);
    void resetDbFolderPath();
    QString defaultPath() const;
    bool pathIsDefault() const;
    bool pathIsDefault(QString p) const;

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
    RamObjectList *steps() const;
    // Asset Groups
    RamObjectList *assetGroups() const;
    // Sequences
    RamObjectList *sequences() const;
    // Shots
    RamItemTable *shots();
    // Assets
    RamItemTable *assets();
    // Pipeline
    RamObjectList *pipeline();
    RamPipe *pipe(RamStep *outputStep, RamStep *inputStep);
    RamObjectList *pipeFiles();
    // Users
    RamObjectList *users() const;

    // Production Tracking
    qint64 timeSpent() const; //seconds
    float estimation() const; //days
    int completionRatio() const; //%
    float latenessRatio() const; //ratio
    float assignedDays() const; //days
    float unassignedDays() const; //days

    static RamProject *project(QString uuid);

    const QDate &deadline() const;
    void setDeadline(const QDate &newDeadline);

    const QString &dbFolderPath() const;
    void setDbFolderPath(const QString &newDbFolderPath);

signals:
    void completionRatioChanged(int);
    void latenessRatioChanged(float);
    void timeSpentChanged(qint64);
    void estimationChanged(float);
    void estimationComputed(RamProject*);

public slots:
    void update() override;
    void updatePath();
    virtual void removeFromDB() override;

    void computeEstimation();

protected:
    virtual QString folderPath() const override;

private slots:
    void userAssigned(const QModelIndex &parent, int first, int last);
    void userUnassigned(const QModelIndex &parent, int first, int last);

private:
    QString m_folderPath;
    QString m_dbFolderPath;
    qreal m_framerate = 24;
    int m_width = 1920;
    int m_height = 1080;
    qreal m_aspectRatio = 1.78;
    RamObjectList *m_steps;
    RamObjectList *m_sequences;
    RamObjectList *m_assetGroups;
    RamItemTable *m_assets;
    RamItemTable *m_shots;
    RamObjectList *m_pipeline;
    RamObjectList *m_pipeFiles;
    RamObjectList *m_users;
    QDate m_deadline;

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
