#ifndef RAMSCHEDULEENTRY_OLD_H
#define RAMSCHEDULEENTRY_OLD_H

#include "ramobject.h"

#include "ramstep.h"
#include "ramuser.h"

class RamScheduleEntry : public RamObject
{
    Q_OBJECT
public:

    // STATIC METHODS //

    static RamScheduleEntry *get(QString uuid);
    static RamScheduleEntry* c(RamObject *o);

    // METHODS

    // User / Step entry
    explicit RamScheduleEntry(RamUser *user, RamStep *step, QDateTime date);

    RamProject *project() const;

    RamUser *user() const;
    const QDateTime date() const;

    RamStep *step() const;
    void setStep(RamStep *newStep);

    virtual QString iconName() const override;

    virtual QVariant roleData(int role) const override;

protected:
    static QHash<QString, RamScheduleEntry*> m_existingObjects;
    RamScheduleEntry(QString uuid);
    virtual QString folderPath() const override { return ""; };

private slots:
    void stepRemoved();

private:
    void construct();
    void connectEvents();

    //RamUser *m_user = nullptr;
    //QDateTime m_date;
};

#endif // RAMSCHEDULEENTRY_OLD_H
