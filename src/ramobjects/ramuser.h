#ifndef RAMUSER_H
#define RAMUSER_H

#include <QStringBuilder>

#include "ramobject.h"
#include "data-models/ramobjectlist.h"

class RamStep;
class RamScheduleEntry;

class RamUser : public RamObject
{
    Q_OBJECT
public:

    // STATIC METHODS //

    static RamUser *getObject(QString uuid, bool constructNew = false);
    static RamUser *c(RamObject *o);

    // METHODS //

    RamUser(QString shortName, QString name);

    virtual void setShortName(const QString &shortName) override;

    UserRole role() const;
    void setRole(const UserRole &role);
    void setRole(const QString role);

    RamObjectList *schedule() const;
    bool isStepAssigned(RamStep *step) const;

    void updatePassword(QString c, QString n);

    virtual QIcon icon() const override;
    virtual QString details() const override;

public slots:
    virtual void edit(bool show = true) override;

protected:
    RamUser(QString uuid);
    virtual QString folderPath() const override;

private:
    void construct();

    RamObjectList *m_schedule;
};

#endif // RAMUSER_H

