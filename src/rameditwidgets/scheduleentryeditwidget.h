#ifndef SCHEDULEENTRYEDITWIDGET_H
#define SCHEDULEENTRYEDITWIDGET_H

#include <QPointer>

#include "duqf-widgets/duqftextedit.h"
#include "duqf-widgets/duscrollarea.h"
#include "ramobjectcombobox.h"
#include "ramobjectview.h"
#include "ramproject.h"
#include "ramuser.h"
#include "ramscheduleentry.h"

class ScheduleEntryEditWidget : public DuScrollArea
{
    Q_OBJECT
public:
    ScheduleEntryEditWidget(RamProject *project, RamUser *user, const QDateTime &date, RamScheduleEntry *entry = nullptr, QWidget *parent = nullptr);

private slots:
    void setStep(RamObject *stepObj);
    void setComment();

private:
    void setupUi();
    void connectEvents();

    QLabel *ui_userLabel;
    QLabel *ui_dateLabel;
    RamObjectComboBox *ui_stepBox;
    DuQFTextEdit *ui_commentEdit;
    RamObjectView *ui_dueTasksList;

    QPointer<RamUser> m_user;
    QPointer<RamProject> m_project;
    QDateTime m_date;
    QPointer<RamScheduleEntry> m_entry;
};

#endif // SCHEDULEENTRYEDITWIDGET_H
