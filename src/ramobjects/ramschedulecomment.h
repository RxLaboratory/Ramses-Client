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

protected:
    static QMap<QString, RamScheduleComment*> m_existingObjects;
    RamScheduleComment(QString uuid);
    virtual QString folderPath() const override { return ""; };

private:
    void construct();

    RamProject *m_project = nullptr;
};

#endif // RAMSCHEDULECOMMENT_H
