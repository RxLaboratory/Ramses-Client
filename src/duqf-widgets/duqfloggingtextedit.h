#ifndef DUQFLOGGINGTEXTEDIT_H
#define DUQFLOGGINGTEXTEDIT_H

#include <QTextEdit>
#include <QScrollBar>
#include "duqf-utils/duqflogger.h"

class DuQFLoggingTextEdit : public QTextEdit
{
    Q_OBJECT
public:
    DuQFLoggingTextEdit(QWidget *parent = nullptr);
    DuQFLoggingTextEdit(DuQFLoggerObject *o, QWidget *parent = nullptr);
    void setLevel(const DuQFLog::LogType &level);

private:
    DuQFLoggerObject *_loggerObject;
    void log(DuQFLog m);
    DuQFLog::LogType _level;
};

#endif // DUQFLOGGINGTEXTEDIT_H
