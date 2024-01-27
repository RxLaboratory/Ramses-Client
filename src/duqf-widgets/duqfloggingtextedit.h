#ifndef DUQFLOGGINGTEXTEDIT_H
#define DUQFLOGGINGTEXTEDIT_H

#include <QTextEdit>
#include "duqf-utils/duqflogger.h"
#include "duqf-widgets/dutextedit.h"

class DuQFLoggingTextEdit : public DuTextEdit
{
public:
    DuQFLoggingTextEdit(QWidget *parent = nullptr);
    DuQFLoggingTextEdit(DuQFLoggerObject *o, QWidget *parent = nullptr);
    void setLevel(const DuQFLog::LogType &level);

public slots:
    void log(DuQFLog m);

private:
    DuQFLoggerObject *_loggerObject;
    DuQFLog::LogType _level;

    int m_contentSizeLimit = 300;
};

#endif // DUQFLOGGINGTEXTEDIT_H
