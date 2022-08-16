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
    RamScheduleComment(QString uuid);

    QDateTime date() const;
    void setDate(const QDateTime &newDate);


private:
    void construct();

    RamProject *m_project = nullptr;
};

#endif // RAMSCHEDULECOMMENT_H
