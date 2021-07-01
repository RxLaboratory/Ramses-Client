#ifndef SCHEDULEMANAGERWIDGET_H
#define SCHEDULEMANAGERWIDGET_H

#include <QWidget>
#include <QDateEdit>
#include <QShortcut>

#include "dbinterface.h"
#include "duqf-utils/guiutils.h"
#include "duqf-widgets/titlebar.h"
#include "data-views/ramscheduletablewidget.h"
#include "data-models/ramscheduletable.h"
#include "data-views/ramobjectlistmenu.h"
#include "data-models/ramschedulefilter.h"
#include "ramses.h"

class ScheduleManagerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ScheduleManagerWidget(QWidget *parent = nullptr);

signals:
    void closeRequested();

protected:
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;

private slots:
    void projectChanged(RamProject *project);
    void userChanged(RamUser *user);

    void assignStep(RamObject *stepObj = nullptr);

    void filterUser(RamObject *userObj, bool filter);
    void filterMe();

    void showMonday(bool show);
    void showTuesday(bool show);
    void showWednesday(bool show);
    void showThursday(bool show);
    void showFriday(bool show);
    void showSaturday(bool show);
    void showSunday(bool show);

    void goTo(QDate date);
    void updateCurrentDate();
    void goToToday();
    void goToDeadline();
    void goToNextMonth();
    void goToPreviousMonth();

private:
    void setupUi();
    void connectEvents();

    TitleBar *ui_titleBar;
    RamScheduleTableWidget *ui_table;
    QDateEdit *ui_startDateEdit;
    QDateEdit *ui_endDateEdit;
    RamObjectListMenu *ui_stepMenu;
    RamObjectListMenu *ui_userMenu;
    QAction *ui_meAction;
    QAction *ui_monday;
    QAction *ui_tuesday;
    QAction *ui_wednesday;
    QAction *ui_thursday;
    QAction *ui_friday;
    QAction *ui_saturday;
    QAction *ui_sunday;
    QToolButton *ui_today;
    QToolButton *ui_deadline;
    QToolButton *ui_nextMonth;
    QToolButton *ui_prevMonth;
    QDateEdit *ui_goTo;

    RamScheduleTable *m_schedule;
    RamScheduleFilter *m_scheduleFilter;

    DBInterface *m_dbi;
};

#endif // SCHEDULEMANAGERWIDGET_H
