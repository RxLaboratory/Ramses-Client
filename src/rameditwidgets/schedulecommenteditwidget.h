#ifndef SCHEDULECOMMENTEDITWIDGET_H
#define SCHEDULECOMMENTEDITWIDGET_H

#include <QPointer>

#include "duqf-widgets/duqftextedit.h"
#include "duqf-widgets/duqfcolorselector.h"
#include "duqf-widgets/duscrollarea.h"
#include "ramproject.h"
#include "ramschedulecomment.h"

class ScheduleCommentEditWidget : public DuScrollArea
{
    Q_OBJECT
public:
    ScheduleCommentEditWidget(
        RamProject *project,
        QDateTime date,
        RamScheduleComment *comment = nullptr,
        QWidget *parent = nullptr );

private slots:
    void setColor(const QColor &color);
    void setComment();

private:
    void setupUi();
    void connectEvents();

    QLabel *ui_dateLabel;
    DuQFTextEdit *ui_commentEdit;
    DuQFColorSelector *ui_colorSelector;

    QPointer<RamProject> m_project;
    QPointer<RamScheduleComment> m_comment;
    QDateTime m_date;

};

#endif // SCHEDULECOMMENTEDITWIDGET_H
