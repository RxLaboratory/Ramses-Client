#ifndef RAMSCHEDULECOMMENT_H
#define RAMSCHEDULECOMMENT_H

#include "ramobject.h"
#include "ramproject.h"

class RamScheduleComment : public RamObject
{
    Q_OBJECT
public:
    explicit RamScheduleComment(RamProject *project, QDateTime date);
    explicit RamScheduleComment(RamProject *project, QDateTime date, QString uuid);


    const QString &comment() const;
    void setComment(const QString &newComment);

    const QColor &color() const;
    void setColor(const QColor &newColor);

    const QDateTime &date() const;
    void setDate(const QDateTime &newDate);

    struct ScheduleCommentStruct toStruct() const;

    static RamScheduleComment *scheduleComment(QString uuid);

public slots:
    void update() override;
    virtual void removeFromDB() override;

private:
    RamProject *m_project = nullptr;
    QString m_comment;
    QColor m_color;
    QDateTime m_date;
};

#endif // RAMSCHEDULECOMMENT_H
