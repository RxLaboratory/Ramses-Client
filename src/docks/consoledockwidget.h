#ifndef CONSOLEDOCKWIDGET_H
#define CONSOLEDOCKWIDGET_H

#include <QPushButton>

#include "duwidgets/duconsolewidget.h"
#include "duwidgets/ducombobox.h"
class ConsoleDockWidget : public QFrame
{
    Q_OBJECT
public:
    explicit ConsoleDockWidget(QWidget *parent = nullptr);
    void clear();

private slots:
    void log(const QString &msg, LogType type = InformationLog, const QString &component = "");
    void setLevel(QVariant l);

private:
    void setupUi();
    void connectEvents();

    DuComboBox *ui_levelBox;
    QPushButton *ui_clearButton;
    DuConsoleWidget *ui_general;
    DuConsoleWidget *ui_daemon;
    DuConsoleWidget *ui_server;
    DuConsoleWidget *ui_db;
};

#endif // CONSOLEDOCKWIDGET_H
