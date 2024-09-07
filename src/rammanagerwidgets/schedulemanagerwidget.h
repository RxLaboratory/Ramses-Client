#ifndef SCHEDULEMANAGERWIDGET_H
#define SCHEDULEMANAGERWIDGET_H

#include <QWidget>
#include <QDateEdit>
#include <QShortcut>
#include <QInputDialog>
#include <QClipboard>

#include "dbinterface.h"
#include "duwidgets/duqftitlebar.h"
#include "ramproject.h"
#include "ramschedulerowheaderview.h"
#include "ramscheduletableview.h"
#include "ramscheduletablemodel.h"
#include "ramobjectmenu.h"
#include "ramschedulefilterproxymodel.h"

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
    void checkRowsFilter();
    void usersInserted(const QModelIndex &parent, int first, int last);

    void projectChanged(RamProject *project);
    void projectUpdated(RamObject*projObj);
    void ramsesReady();
    void changeUserRole(RamUser::UserRole role);

    void addEntry(RamObject *stepObj = nullptr);
    bool clearSelectedEntries();

    void filterUser(RamObject *user, bool filter);
    void filterMe();
    void addRow();
    void removeSelectedRows();

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

    void copyEntries();
    void cutEntries();
    void pasteEntries();

    void contextMenuRequested(QPoint p);

private:
    void setupUi();
    void connectEvents();
    void loadSettings();

    void changeProject();
    bool m_projectChanged = false;

    QModelIndexList m_entryClipBoard;
    Qt::DropAction m_clipboardAction = Qt::CopyAction;

    DuQFTitleBar *ui_titleBar;
    QAction *ui_actionShowDetails;
    RamScheduleTableView *ui_table;
    RamScheduleRowHeaderView *ui_rowHeader;
    QDateEdit *ui_startDateEdit;
    QDateEdit *ui_endDateEdit;
    RamObjectMenu *ui_addEntryMenu;
    RamObjectMenu *ui_replaceEntryMenu;
    QToolButton *ui_rowsButton;
    RamObjectMenu *ui_rowsMenu;

    QAction *ui_addRowAction;
    QAction *ui_removeRowAction;
    QAction *ui_clearAction;
    QAction *ui_meAction;

    QAction *ui_monday;
    QAction *ui_tuesday;
    QAction *ui_wednesday;
    QAction *ui_thursday;
    QAction *ui_friday;
    QAction *ui_saturday;
    QAction *ui_sunday;

    QAction *ui_copyEntries;
    QAction *ui_cutEntries;
    QAction *ui_pasteEntries;

    QToolButton *ui_today;
    QToolButton *ui_deadline;
    QToolButton *ui_nextMonth;
    QToolButton *ui_prevMonth;
    QDateEdit *ui_goTo;
    QLabel *ui_timeRemaining;
    DuMenu *ui_entriesMenu;

    RamScheduleTableModel *m_schedule;
    RamScheduleFilterProxyModel *m_scheduleFilter;

    RamProject *m_project = nullptr;

    DBInterface *m_dbi;
};

#endif // SCHEDULEMANAGERWIDGET_H
