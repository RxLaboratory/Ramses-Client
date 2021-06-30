#ifndef SCHEDULEENTRYEDITWIDGET_H
#define SCHEDULEENTRYEDITWIDGET_H

#include <QDateEdit>

#include "objecteditwidget.h"
#include "ramscheduleentry.h"
#include "ramobjectlistcombobox.h"

class ScheduleEntryEditWidget : public ObjectEditWidget
{
    Q_OBJECT
public:
    ScheduleEntryEditWidget(RamScheduleEntry *entry = nullptr, QWidget *parent = nullptr);

public slots:
    void setObject(RamObject *obj) override;

protected slots:
    void update() override;

private:
    RamScheduleEntry *m_entry = nullptr;

    void setupUi();
    void connectEvents();

    RamObjectListComboBox *ui_userBox;
    RamObjectListComboBox *ui_stepBox;
    QDateEdit *ui_dateEdit;
    QComboBox *ui_ampmBox;
};

#endif // SCHEDULEENTRYEDITWIDGET_H
