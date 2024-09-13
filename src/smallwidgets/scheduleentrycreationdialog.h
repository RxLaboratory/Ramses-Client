#ifndef SCHEDULEENTRYCREATIONDIALOG_H
#define SCHEDULEENTRYCREATIONDIALOG_H

#include "duwidgets/ducolorselector.h"
#include "qdialogbuttonbox.h"
#include <QDialog>

#include "duwidgets/dulineedit.h"
#include "duwidgets/durichtextedit.h"

class ScheduleEntryCreationDialog : public QDialog
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
