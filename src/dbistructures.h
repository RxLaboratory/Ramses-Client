#ifndef DBISTRUCTURES_H
#define DBISTRUCTURES_H

#include <QString>
#include <QDateTime>

struct ScheduleEntryStruct {
    QString uuid = "";
    QString stepUuid = "";
    QString userUuid = "";
    QDateTime date = QDateTime();
    QString comment = "";
};

#endif // DBISTRUCTURES_H
