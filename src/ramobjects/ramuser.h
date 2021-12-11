#ifndef RAMUSER_H
#define RAMUSER_H

#include <QStringBuilder>

#include "ramobject.h"

class RamObjectList;
class RamStep;

class RamUser : public RamObject
{
    Q_OBJECT
public:
    explicit RamUser(QString shortName, QString name = "", QString uuid = "");
    ~RamUser();

    UserRole role() const;
    void setRole(const UserRole &role);
    void setRole(const QString role);

    void updatePassword(QString c, QString n);

    RamObjectList *schedule() const;
    bool isStepAssigned(RamStep *step) const;

    static RamUser *user(QString uuid);

    const QColor &color() const;
    void setColor(const QColor &newColor);

public slots:
    void update() override;
    virtual void edit(bool show = true) override;
    virtual void removeFromDB() override;

private slots:
    void scheduleChanged(RamObject *entryObj);

protected:
    virtual QString folderPath() const override;   

private:
    UserRole m_role;
    RamObjectList *m_schedule;

    QColor m_color;
};

#endif // RAMUSER_H

