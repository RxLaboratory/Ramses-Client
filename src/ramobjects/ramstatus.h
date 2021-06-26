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
    explicit RamStatus(RamUser *user, RamState *state, RamStep *step, RamItem *item, QString uuid = "");
    ~RamStatus();

    int completionRatio() const;
    void setCompletionRatio(int completionRatio);

    RamUser *user() const;

    RamState *state() const;
    void setState(RamState *state);

    int version() const;
    void setVersion(int version);

    RamStep *step() const;

    RamItem *item() const;

    QDateTime date() const;
    void setDate(const QDateTime &date);

    static RamStatus *status(QString uuid);

    bool isPublished() const;
    void setPublished(bool published);

    RamUser *assignedUser() const;
    void assignUser(RamUser *assignedUser);

public slots:
    void update() override;
    virtual void edit(bool show = true) override;

protected:
    virtual QString folderPath() const override;

private slots:
    void statusUpdated(RamState *state, int completion, int version, QString comment);

private:
    int m_completionRatio = 50;
    RamUser *m_user;
    RamState *m_state;
    RamItem *m_item;
    int m_version = 1;
    RamStep *m_step;
    QDateTime m_date;
    bool m_published = false;
    RamUser* m_assignedUser = nullptr;
    StatusEditWidget *m_editWidget;
};

#endif // RAMSTATUS_H
