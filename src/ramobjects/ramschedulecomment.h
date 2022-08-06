#ifndef RAMSCHEDULECOMMENT_H
#define RAMSCHEDULECOMMENT_H

#include "ramproject.h"

class RamProject;

class RamScheduleComment : public RamObject
{
    Q_OBJECT
public:

    // STATIC METHODS //

    static RamScheduleComment *getObject(QString uuid, bool constructNew = false);

    // METHODS //

    RamScheduleComment(RamProject *project);

    QDateTime date() const;
    void setDate(const QDateTime &newDate);

protected:
    RamScheduleComment(QString uuid);

private:
    void construct();

    RamProject *m_project = nullptr;
};

#endif // RAMSCHEDULECOMMENT_H
