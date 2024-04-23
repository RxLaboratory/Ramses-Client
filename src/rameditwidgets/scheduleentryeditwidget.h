#ifndef SCHEDULEENTRYEDITWIDGET_H
#define SCHEDULEENTRYEDITWIDGET_H

#include <QPointer>

#include "duqf-widgets/dulineedit.h"
#include "duqf-widgets/duqfcolorselector.h"
#include "duqf-widgets/duqftextedit.h"
#include "ramobjectcombobox.h"
#include "ramuser.h"
#include "ramscheduleentry.h"
#include "objecteditwidget.h"

class ScheduleEntryEditWidget : public ObjectEditWidget
{
    Q_OBJECT
public:
    ScheduleEntryEditWidget(RamScheduleEntry *entry, QWidget *parent = nullptr);

protected:
    virtual void reInit(RamObject *o) override;

private:
    void setupUi();
    void connectEvents();

    DuLineEdit *ui_titleEdit;
    DuQFColorSelector *ui_colorSelector;
    RamObjectComboBox *ui_stepBox;
    QDateEdit *ui_dateEdit;
    RamObjectComboBox *ui_rowBox;
    DuQFTextEdit *ui_commentEdit;
    QToolButton *ui_removeButton;

    QPointer<RamScheduleEntry> m_entry;
};

#endif // SCHEDULEENTRYEDITWIDGET_H
