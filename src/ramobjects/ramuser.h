#ifndef RAMUSER_H
#define RAMUSER_H

#include <QStringBuilder>

#include "ramobject.h"

class RamStep;
class RamScheduleEntry;
template<typename RO> class RamObjectList;

class RamUser : public RamObject
{
    Q_OBJECT
public:

    // STATIC METHODS //

    static RamUser *getObject(QString uuid, bool constructNew = false);

    // METHODS //

    RamUser(QString shortName, QString name);

    virtual void setShortName(const QString &shortName) override;

    UserRole role() const;
    void setRole(const UserRole &role);
    void setRole(const QString role);

    QColor color() const;
    void setColor(const QColor &newColor);

    RamObjectList<RamScheduleEntry*> *schedule() const;
    bool isStepAssigned(RamStep *step) const;

    void updatePassword(QString c, QString n);

public slots:
    virtual void edit(bool show = true) override;

protected:
    RamUser(QString uuid);
    virtual QString folderPath() const override;

    // Reimplement data methods to encrypt user data
    virtual QString dataString() const override;
    virtual void setDataString(QString data) override;
    virtual void createData(QString data) override;

private:
    void construct();

    RamObjectList<RamScheduleEntry*> *m_schedule;
};

#endif // RAMUSER_H

