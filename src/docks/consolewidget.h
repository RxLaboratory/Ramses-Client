#ifndef CONSOLEWIDGET_H
#define CONSOLEWIDGET_H

#include <QFrame>

#include "duwidgets/duqfloggingtextedit.h"
#include "duutils/duqflogger.h"
#include "duwidgets/ducombobox.h"
#include "qpushbutton.h"

class ConsoleWidget : public QFrame
{
    Q_OBJECT
public:
    explicit ConsoleWidget(QWidget *parent = nullptr);
    explicit ConsoleWidget(DuQFLoggerObject *o, QWidget *parent = nullptr);

signals:

private slots:
    void levelIndexChanged(int index);
    void clear();

private:
    void setupUi(DuQFLoggerObject *o = nullptr);
    void connectEvents();

    DuQFLoggingTextEdit *ui_consoleEdit;
    DuComboBox *ui_levelBox;
    QPushButton *ui_clearButton;
};

#endif // CONSOLEWIDGET_H
