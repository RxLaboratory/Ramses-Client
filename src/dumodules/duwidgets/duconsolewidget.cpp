#include "duconsolewidget.h"

#include <QScrollBar>
#include <QTime>

#include "duapp/dusettings.h"
#include "duapp/duui.h"

DuConsoleWidget::DuConsoleWidget(QWidget *parent):
    DuTextEdit(parent)
{
    _level = DebugLog;
}

void DuConsoleWidget::setLevel(LogType level)
{
    _level = level;
}

void DuConsoleWidget::log(const QString &msg, LogType type, const QString &component)
{
    if (type < _level) return;

    const bool atBottom = this->verticalScrollBar()->value() == this->verticalScrollBar()->maximum();
    QTextDocument* doc = this->document();
    QTextCursor cursor(doc);
    cursor.movePosition(QTextCursor::End);
    cursor.beginEditBlock();
    cursor.insertBlock();

    // Time
    QTextCharFormat f;
    f.setForeground( QColor(109,109,109) );
    f.setFontWeight(300);
    cursor.insertText(QTime::currentTime().toString("[hh:mm:ss.zzz]: "), f);

    QColor fgColor = DuSettings::i()->get(DuSettings::UI_ForegroundColor).value<QColor>();

    // Title
    QColor c;
    QString t;
    switch(type){
    case DataLog:
        t = "(Data) " + component;
        c = DuUI::pushColor(fgColor);
        break;
    case DebugLog:
        t = "(Debug) " + component;
        c = QColor(95,104,161); // blue
        break;
    case InformationLog:
        t = "(Information) " + component;
        c = fgColor;
        break;
    case WarningLog:
        t = "/!\\ Warning: " + component;
        c = QColor(236,215,24); // Yellow
        break;
    case CriticalLog:
        t = " --- !!! Critical: " + component;
        c = QColor(249,105,105); // Red
        break;
    case FatalLog:
        t = " === Fatal === " + component;
        c = QColor(213,136,241); // Purple
        break;
    }
    f.setForeground( c );
    f.setFontWeight(800);
    f.setFontItalic(true);
    cursor.insertText(t + "\n", f);

    // Content
    if (type != DataLog) f.setForeground(fgColor);
    f.setFontWeight(400);
    f.setFontItalic(false);
    cursor.insertText(msg, f);

    cursor.endEditBlock();

    //scroll scrollarea to bottom if it was at bottom when we started
    //(we don't want to force scrolling to bottom if user is looking at a
    //higher position)
    if (atBottom) {
        this->verticalScrollBar()->setValue(
            this->verticalScrollBar()->maximum()
            );
    }
}
