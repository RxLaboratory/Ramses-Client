#ifndef CONSOLEWIDGET_H
#define CONSOLEWIDGET_H

#include <QWidget>

#include "duqf-widgets/duqfloggingtextedit.h"
#include "duqf-utils/duqflogger.h"

class ConsoleWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ConsoleWidget(QWidget *parent = nullptr);
    explicit ConsoleWidget(DuQFLoggerObject *o, QWidget *parent = nullptr);

signals:

private slots:
    void levelIndexChanged(int index);

private:
    void setupUi(DuQFLoggerObject *o = nullptr);
    void connectEvents();

    DuQFLoggingTextEdit *ui_consoleEdit;
    QComboBox *ui_levelBox;
};

#endif // CONSOLEWIDGET_H
