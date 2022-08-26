#ifndef DUQFLOGGINGTEXTEDIT_H
#define DUQFLOGGINGTEXTEDIT_H

#include <QTextEdit>
#include <QScrollBar>
#include "duqf-utils/duqflogger.h"

class DuQFLoggingTextEdit : public QTextEdit
{
public:
    DuQFLoggingTextEdit(QWidget *parent = nullptr);
    DuQFLoggingTextEdit(DuQFLoggerObject *o, QWidget *parent = nullptr);
    void setLevel(const DuQFLog::LogType &level);

private slots:
    void log(DuQFLog m);

private:
    DuQFLoggerObject *_loggerObject;
    DuQFLog::LogType _level;
};

#endif // DUQFLOGGINGTEXTEDIT_H
