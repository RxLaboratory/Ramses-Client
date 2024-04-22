#ifndef SCHEDULEROWEDITWIDGET_H
#define SCHEDULEROWEDITWIDGET_H

#include "objecteditwidget.h"
#include "ramobjectcombobox.h"
#include "ramschedulerow.h"

class ScheduleRowEditWidget : public ObjectEditWidget
{
    Q_OBJECT
public:
    ScheduleRowEditWidget(QWidget *parent = nullptr);
    ScheduleRowEditWidget(RamScheduleRow *row, QWidget *parent = nullptr);

    RamScheduleRow *row();

protected:
    virtual void reInit(RamObject *o) override;

private slots:
    void setUser(RamObject *usrObj);

private:
    void setupUi();
    void connectEvents();

    RamObjectComboBox *ui_userBox;

    RamScheduleRow *m_row = nullptr;
};

#endif // SCHEDULEROWEDITWIDGET_H
