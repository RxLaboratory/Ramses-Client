#include "dutextedit.h"
#include "duscrollbar.h"

#include "duqf-app/duui.h"

DuTextEdit::DuTextEdit(QWidget *parent):
    QTextEdit(parent)
{
    setVerticalScrollBar(new DuScrollBar(Qt::Vertical, this));
    setHorizontalScrollBar(new DuScrollBar(Qt::Horizontal, this));

    setStyleSheet(DuUI::css("DuTextEdit"));
}

void DuTextEdit::setNumLines(int n)
{
    QTextDocument *doc = this->document();
    QFontMetrics fm (doc->defaultFont());
    QMargins margins = this->contentsMargins();
    int nHeight = fm.lineSpacing () * n +
                  (doc->documentMargin() + this->frameWidth()) * 2 +
                  margins.top() + margins.bottom();
    this->setFixedHeight(nHeight);
}

void DuTextEdit::mouseMoveEvent(QMouseEvent *event)
{
    if (_dragging)
    {
        QPoint newPos = event->globalPos();
        QPoint _delta = newPos - _initialDragPos;
        this->horizontalScrollBar()->setValue( this->horizontalScrollBar()->value() - _delta.x() );
        this->verticalScrollBar()->setValue( this->verticalScrollBar()->value() - _delta.y() );
        _initialDragPos = newPos;
        event->accept();
        return;
    }
    QTextEdit::mouseMoveEvent(event);
}

void DuTextEdit::mousePressEvent(QMouseEvent *event)
{
    // Middle click for dragging view
    if (event->button() == Qt::MiddleButton)
    {
        _initialDragPos = event->globalPos();
        _dragging = true;
        event->accept();
        return;
    }
    QTextEdit::mousePressEvent(event);
}

void DuTextEdit::mouseReleaseEvent(QMouseEvent *event)
{
    // Middle click for dragging view
    if (event->button() == Qt::MiddleButton)
    {
        _dragging = false;
        event->accept();
        return;
    }
    QTextEdit::mouseReleaseEvent(event);
}
