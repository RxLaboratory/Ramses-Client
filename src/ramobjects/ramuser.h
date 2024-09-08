#ifndef RAMUSER_H
#define RAMUSER_H

#include <QStringBuilder>

#include "ramobject.h"
#include "ramscheduleentrymodel.h"
#include "ramjsonusereditwidget.h"

class RamStep;
class RamScheduleEntry;

class RamUser : public RamObject
{
    Q_OBJECT
public:

    // STATIC METHODS //

    static RamUser *get(QString uuid);
    static RamUser *c(RamObject *o);

    // THE JSON DATA KEYS and ENUM VALUES

    static const QString ENUMVALUE_Admin;
    static const QString ENUMVALUE_ProjectAdmin;
    static const QString ENUMVALUE_Lead;
    static const QString ENUMVALUE_Standard;

    // METHODS //

    RamUser(QString shortName, QString name);

    QJsonObject toJson() const override;
    void loadJson(const QJsonObject &obj) override;

    UserRole role() const;
    bool setRole(const UserRole &role);
    bool setRole(const QString role);

    DBTableModel *schedule() const;
    bool isStepAssigned(RamStep *step) const;

    virtual QString iconName() const override;
    virtual QString details() const override;

public slots:
    virtual void edit(bool show = true) override;

protected:
    static QHash<QString, RamUser*> m_existingObjects;
    RamUser(QString uuid);
    virtual QString folderPath() const override;

    QPointer<RamJsonUserEditWidget> ui_jsonEditWidget = nullptr;

private:
    void construct();

    RamScheduleEntryModel *m_schedule;
    QString m_role;
};

#endif // RAMUSER_H

