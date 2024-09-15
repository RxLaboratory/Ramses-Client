#ifndef DUCONSOLEWIDGET_H
#define DUCONSOLEWIDGET_H

#include "duwidgets/dutextedit.h"
#include "enums.h"

class DuConsoleWidget : public DuTextEdit
{
    Q_OBJECT
public:
    DuConsoleWidget(QWidget *parent = nullptr);
    void setLevel(LogType level);

public slots:
    void log(const QString &msg, LogType type = InformationLog, const QString &component = "");

private:
    LogType _level;
};

#endif // DUCONSOLEWIDGET_H
