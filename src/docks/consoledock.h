#ifndef CONSOLEDOCK_H
#define CONSOLEDOCK_H

#include <QPushButton>

#include "duwidgets/duconsolewidget.h"
#include "duwidgets/ducombobox.h"
class ConsoleDock : public QFrame
{
    Q_OBJECT
public:
    explicit ConsoleDock(QWidget *parent = nullptr);
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

#endif // CONSOLEDOCK_H
