#ifndef SCHEDULEENTRYCREATIONDIALOG_H
#define SCHEDULEENTRYCREATIONDIALOG_H

#include "duwidgets/ducolorselector.h"
#include "qdialogbuttonbox.h"

#include "duwidgets/dulineedit.h"
#include "duwidgets/durichtextedit.h"
#include "duwidgets/dudialog.h"

class ScheduleEntryCreationDialog : public DuDialog
{
    Q_OBJECT
public:
    ScheduleEntryCreationDialog( QWidget *parent = nullptr );

    QString title() const;
    QString comment() const;
    QColor color() const;

private:
    DuLineEdit *ui_titleEdit;
    DuRichTextEdit *ui_commentEdit;
    DuColorSelector *ui_colorSelector;
    QDialogButtonBox *ui_buttonBox;
};

#endif // SCHEDULEENTRYCREATIONDIALOG_H
