#ifndef DBMANAGERWIDGET_H
#define DBMANAGERWIDGET_H

#include "duwidgets/autoselectspinbox.h"
#include "qcheckbox.h"
#include "qpushbutton.h"
#include "qtextedit.h"

class DBManagerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DBManagerWidget(QWidget *parent = nullptr);

signals:

private slots:
    void clean();
    void cancel();
    void accept();

private:
    void setupUi();
    void connectEvents();

    QPushButton *ui_cleanButton;
    QPushButton *ui_cancelCleanButton;
    QPushButton *ui_acceptCleanButton;
    QTextEdit *ui_reportEdit;
    AutoSelectSpinBox *ui_deleteFromEdit;
    QCheckBox *ui_deleteCheckBox;

};

#endif // DBMANAGERWIDGET_H
