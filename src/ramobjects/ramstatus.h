#ifndef RAMSTATUS_H
#define RAMSTATUS_H

#include <QDateTime>

#include "ramuser.h"
#include "ramstate.h"
#include "ramstep.h"
#include "ramfilemetadatamanager.h"
#include "ramnamemanager.h"

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
    ~RamStatus();

    int completionRatio() const;
    void setCompletionRatio(int completionRatio);

    RamUser *user() const;

    RamState *state() const;
    void setState(RamState *state);

    QStringList mainFiles() const;
    QString createFileFromTemplate(QString filePath) const;

    int version() const;
    QStringList versionFiles() const;
    QStringList versionFiles(QString resource) const;
    int latestVersion() const;
    int latestVersion(QString resource) const;
    void setVersion(int version);
    QString restoreVersionFile(QString fileName) const;

    RamStep *step() const;

    RamItem *item() const;

    QDateTime date() const;
    void setDate(const QDateTime &date);

    bool isPublished() const;
    bool checkPublished(int version = -1) const;
    QStringList publishedVersionFolders() const;
    QStringList publishedFiles( QString versionFolder ) const;
    QStringList publishedFiles( QString resource, QString versionFolder ) const;
    void setPublished(bool published);

    QString previewImagePath() const;
    QStringList previewFiles() const;

    RamUser *assignedUser() const;
    void assignUser(RamUser *assignedUser);

    qint64 timeSpent(); // seconds
    void setTimeSpent(const float &ts);
    bool isTimeSpentManual() const;

    Difficulty difficulty() const;
    void setDifficulty(Difficulty newDifficulty);

    float estimation() const; // days
    float autoEstimation() const; // days
    float autoEstimation(int difficulty) const; // days
    float latenessRatio() const;
    void setEstimation(float newEstimation);

    static RamStatus *status(QString uuid);
    static RamStatus *copy(RamStatus *other, RamUser *user);
    static float hoursToDays(int hours);
    static int daysToHours(float days);

public slots:
    void update() override;
    virtual void edit(bool show = true) override;
    virtual void removeFromDB() override;

protected:
    virtual QString folderPath() const override;

private slots:
    void statusUpdated(RamState *state, int completion, int version, QString comment);
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
    float m_estimation = -1.0;

    StatusEditWidget *ui_editWidget;

    QMetaObject::Connection m_stateConnection;
    QMetaObject::Connection m_assignedUserConnection;

};

bool versionFolderSorter(QString a, QString b);
int getVersionFolderRank(QString folder);

#endif // RAMSTATUS_H
