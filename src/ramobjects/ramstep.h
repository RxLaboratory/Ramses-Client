#ifndef RAMSTEP_H
#define RAMSTEP_H

#include <QDesktopServices>

#include "ramobject.h"
#include "ramuser.h"
#include "ramapplication.h"
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

    bool isTemplate() const;
    RamStep *createFromTemplate(RamProject *project);
    RamStep *createFromTemplate(QString projectUuid);

    Type type() const;
    void setType(const Type &type);
    void setType(QString type);

    RamProject *project() const;
    void setProject( RamProject *project );

    RamObjectList *users() const;
    RamObjectList *applications() const;

    void openFile(QString filePath) const;
    QStringList publishedTemplates() const;
    QString templateFile(QString templateFileName) const;

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

signals:
    void estimationChanged(RamStep*);

public slots:
    void update() override;
    virtual void edit(bool show = true) override;
    virtual void removeFromDB() override;

    void computeEstimation();

protected:
    virtual QString folderPath() const override;

private slots:
    void userAssigned(const QModelIndex &parent, int first, int last);
    void userUnassigned(const QModelIndex &parent, int first, int last);
    void applicationAssigned(const QModelIndex &parent, int first, int last);
    void applicationUnassigned(const QModelIndex &parent, int first, int last);


private:
    bool m_template;
    Type m_type;
    QColor m_color;
    RamProject *m_project;
    RamObjectList *m_users;
    RamObjectList *m_applications;

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

};

#endif // RAMSTEP_H
