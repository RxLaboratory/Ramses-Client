#ifndef SCHEDULEMANAGERWIDGET_H
#define SCHEDULEMANAGERWIDGET_H

#include <QWidget>
#include <QDateEdit>

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

    void assignStep(RamObject *stepObj = nullptr);

    void filterUser(RamObject *userObj, bool filter);
    void filterMe();

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

    RamScheduleTable *m_schedule;
    RamScheduleFilter *m_scheduleFilter;

    DBInterface *m_dbi;
};

#endif // SCHEDULEMANAGERWIDGET_H
