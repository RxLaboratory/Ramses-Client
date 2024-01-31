#ifndef SCHEDULEENTRYEDITWIDGET_H
#define SCHEDULEENTRYEDITWIDGET_H

#include "duqf-widgets/duqftextedit.h"
#include "duqf-widgets/duscrollarea.h"
#include "ramobjectcombobox.h"
#include "ramproject.h"
#include "ramuser.h"

class ScheduleEntryEditWidget : public DuScrollArea
{
    Q_OBJECT
public:
    ScheduleEntryEditWidget(RamProject *project, RamUser *user, const QDateTime &date, QWidget *parent = nullptr);

private:
    void setupUi();
    void connectEvents();

    QLabel *ui_userLabel;
    QLabel *ui_dateLabel;
    RamObjectComboBox *ui_stepBox;
    DuQFTextEdit *ui_commentEdit;

    RamUser *m_user;
    RamProject *m_project;
    QDateTime m_date;
};

#endif // SCHEDULEENTRYEDITWIDGET_H
