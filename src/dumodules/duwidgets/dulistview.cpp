#include "dulistview.h"

#include <QMouseEvent>

#include "duwidgets/duscrollbar.h"

DuListView::DuListView(QWidget *parent):
    QListView (parent)
{
    this->setFrameStyle(QFrame::NoFrame);
    this->setSelectionMode(QAbstractItemView::ExtendedSelection);
    this->setVerticalScrollBar(new DuScrollBar(Qt::Vertical, this));
    this->setHorizontalScrollBar(new DuScrollBar(Qt::Horizontal, this));
    this->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    this->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
}

void DuListView::mouseMoveEvent(QMouseEvent *event)
{
    if (!m_middlePressed) {
        event->ignore();
        QListView::mouseMoveEvent(event);
        return;
    }


    QPoint newPos = event->globalPos();
    QPoint _delta = newPos - m_initialDragPos;

    if (!m_dragging) {
        int distance = _delta.manhattanLength();
        if (distance > 10)
            m_dragging = true;
    }

    if (m_dragging)
    {
        this->horizontalScrollBar()->setValue( this->horizontalScrollBar()->value() - _delta.x() );
        this->verticalScrollBar()->setValue( this->verticalScrollBar()->value() - _delta.y() );
        m_initialDragPos = newPos;
        event->accept();
        return;
    }

    event->ignore();
    QListView::mouseMoveEvent(event);
}

void DuListView::mousePressEvent(QMouseEvent *event)
{
    // Middle click for dragging view
    if (event->button() == Qt::MiddleButton)
    {
        m_initialDragPos = event->globalPos();
        m_middlePressed = true;
        event->accept();
        return;
    }

    event->ignore();
    QListView::mousePressEvent(event);
}

void DuListView::mouseReleaseEvent(QMouseEvent *event)
{
    if (!m_middlePressed || event->button() != Qt::MiddleButton) {
        event->ignore();
        QListView::mouseReleaseEvent(event);
        return;
    }

    m_middlePressed = false;

    // Middle click for dragging view
    if (m_dragging)
    {
        m_dragging = false;
        event->accept();
        return;
    }

    // Emit the item clicked
    QModelIndex i = indexAt(event->pos());
    if (i.isValid()) {
        emit middleClicked(i);
        event->accept();
        return;
    }

    event->ignore();
    QListView::mouseReleaseEvent(event);
}
