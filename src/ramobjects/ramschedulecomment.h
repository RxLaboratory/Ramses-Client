#ifndef RAMSCHEDULECOMMENT_H
#define RAMSCHEDULECOMMENT_H

#include "ramproject.h"

class RamProject;

class RamScheduleComment : public RamObject
{
    Q_OBJECT
public:

    // STATIC METHODS //

    static RamScheduleComment *get(QString uuid);
    static RamScheduleComment *c(RamObject *o);

    // METHODS //

    RamScheduleComment(RamProject *project);

    QDateTime date() const;
    void setDate(const QDateTime &newDate);

    virtual QVariant roleData(int role) const override;

protected:
    static QMap<QString, RamScheduleComment*> m_existingObjects;
    RamScheduleComment(QString uuid);
    virtual QString folderPath() const override { return ""; };

    static QFrame *ui_editWidget;

private:
    void construct();

    RamProject *m_project = nullptr;
};

#endif // RAMSCHEDULECOMMENT_H
