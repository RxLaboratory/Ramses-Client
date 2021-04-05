#include "duqfloggingtextedit.h"

DuQFLoggingTextEdit::DuQFLoggingTextEdit(QWidget *parent): QTextEdit(parent)
{
    this->setReadOnly(true);
    _loggerObject = nullptr;
    _level = DuQFLog::Debug;

    connect(DuQFLogger::instance(), &DuQFLogger::newLog, this, &DuQFLoggingTextEdit::log);
}

DuQFLoggingTextEdit::DuQFLoggingTextEdit(DuQFLoggerObject *o, QWidget *parent): QTextEdit(parent)
{
    _loggerObject = o;
    _level = DuQFLog::Debug;
    connect(o, &DuQFLoggerObject::newLog, this, &DuQFLoggingTextEdit::log);
}

void DuQFLoggingTextEdit::log(DuQFLog m)
{
    //ignore debug messages on release
#ifndef QT_DEBUG
    if (m.type() < DuQFLog::Information) return;
#endif

    if (m.type() < _level) return;

    // Time
    this->setTextColor(QColor(109,109,109));
    this->setFontWeight(300);
    this->append(m.timeString());
    this->moveCursor(QTextCursor::End);

    // Title
    QColor c = m.color();
    if (c.isValid()) this->setTextColor(c);
    else this->setTextColor(QColor(227,227, 227));
    this->setFontWeight(800);
    this->setFontItalic(true);
    this->insertPlainText(m.typeString() + " says: ");

    // Content
    this->setFontWeight(400);
    this->setFontItalic(false);
    this->insertPlainText(m.message());
    this->verticalScrollBar()->setSliderPosition(this->verticalScrollBar()->maximum());
}

void DuQFLoggingTextEdit::setLevel(const DuQFLog::LogType &level)
{
    _level = level;
}
