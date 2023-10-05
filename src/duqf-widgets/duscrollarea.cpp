#include "duscrollarea.h"
#include "duscrollbar.h"

DuScrollArea::DuScrollArea(QWidget *parent):
    QScrollArea(parent)
{
    setVerticalScrollBar(new DuScrollBar(Qt::Vertical, this));
    setHorizontalScrollBar(new DuScrollBar(Qt::Horizontal, this));
}

void DuScrollArea::mouseMoveEvent(QMouseEvent *event)
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
    QScrollArea::mouseMoveEvent(event);
}

void DuScrollArea::mousePressEvent(QMouseEvent *event)
{
    // Middle click for dragging view
    if (event->button() == Qt::MiddleButton)
    {
        _initialDragPos = event->globalPos();
        _dragging = true;
        event->accept();
        return;
    }
    QScrollArea::mousePressEvent(event);
}

void DuScrollArea::mouseReleaseEvent(QMouseEvent *event)
{
    // Middle click for dragging view
    if (event->button() == Qt::MiddleButton)
    {
        _dragging = false;
        event->accept();
        return;
    }
    QScrollArea::mouseReleaseEvent(event);
}
