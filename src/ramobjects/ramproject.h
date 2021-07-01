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

    void setFolderPath(const QString &folderPath);
    QString defaultPath() const;
    bool pathIsDefault() const;

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

    // Production Tracking
    qint64 timeSpent() const; //seconds
    float estimation() const; //days
    float completionRatio() const;
    float latenessRatio() const;

    static RamProject *project(QString uuid);

    const QDate &deadline() const;
    void setDeadline(const QDate &newDeadline);

public slots:
    void update() override;
    virtual void removeFromDB() override;

    void computeEstimation();


protected:
    virtual QString folderPath() const override;

private:
    QString m_folderPath;
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
    QDate m_deadline;

    qint64 m_timeSpent = 0;
    float m_estimation = 0;
    float m_completionRatio = 0;
    float m_latenessRatio = 0;
};

#endif // RAMPROJECT_H
