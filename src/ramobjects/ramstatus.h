#ifndef RAMSTATUS_H
#define RAMSTATUS_H

#include <QDateTime>

#include "ramuser.h"
#include "ramstate.h"
#include "ramstep.h"

class RamItem;
class StatusEditWidget;

class RamStatus : public RamObject
{
    Q_OBJECT
public:
    enum Difficulty {
        VeryEasy = 0,
        Easy = 1,
        Medium = 2,
        Hard = 3,
        VeryHard = 4
    };
    Q_ENUM(Difficulty)

    explicit RamStatus(RamUser *user, RamState *state, RamStep *step, RamItem *item, bool computeEstimation = true, QString uuid = "");

    virtual QString shortName() const override;
    virtual QString name() const override;

    int completionRatio() const;
    void setCompletionRatio(int completionRatio);

    RamUser *user() const;

    RamState *state() const;
    void setState(RamState *state);

    RamWorkingFolder workingFolder() const;
    QString createFileFromTemplate(QString filePath) const;
    QString createFileFromResource(QString filePath) const;

    int version() const;
    void setVersion(int version);
    QString restoreVersionFile(QString fileName) const;

    RamStep *step() const;

    RamItem *item() const;

    QDateTime date() const;
    void setDate(const QDateTime &date);

    bool isPublished() const;
    void setPublished(bool published);

    QString previewImagePath() const;

    RamUser *assignedUser() const;
    void assignUser(RamUser *assignedUser);

    qint64 timeSpent(); // seconds
    void setTimeSpent(const float &ts);
    bool isTimeSpentManual() const;

    Difficulty difficulty() const;
    void setDifficulty(Difficulty newDifficulty);

    float goal() const; // days
    void setGoal(float newGoal);
    float estimation() const; // days
    float estimation(int difficulty) const; // days
    float latenessRatio() const;
    void setUseAutoEstimation(bool newAutoEstimation);
    bool useAutoEstimation() const;

    static RamStatus *status(QString uuid);
    static RamStatus *copy(RamStatus *other, RamUser *user);
    static float hoursToDays(int hours);
    static int daysToHours(float days);

public slots:
    virtual void edit(bool show = true) override;

protected:
    virtual QString folderPath() const override;

private slots:
    void stateRemoved();
    void userRemoved();
    void assignedUserRemoved();

private:  
    int m_completionRatio = 50;
    RamUser *m_user;
    RamState *m_state;
    RamItem *m_item;
    int m_version = 1;
    RamStep *m_step;
    QDateTime m_date;
    qint64 m_timeSpent = 0;
    bool m_manualTimeSpent = false;
    bool m_published = false;
    RamUser* m_assignedUser = nullptr;
    Difficulty m_difficulty = Medium;
    float m_goal = -1.0;
    bool m_useAutoEstimation = true;

    StatusEditWidget *ui_editWidget;

    QMetaObject::Connection m_stateConnection;
    QMetaObject::Connection m_assignedUserConnection;

};

#endif // RAMSTATUS_H
