#include "dulistwidget.h"
#include "duapp/duui.h"
#include "duwidgets/duscrollbar.h"
#include "qevent.h"

DuListWidget::DuListWidget(QWidget *parent):
    QListWidget (parent)
{
    this->setFrameStyle(QFrame::NoFrame);
    this->setSelectionMode(QAbstractItemView::ExtendedSelection);
    this->setStyleSheet(DuUI::css("DuListView"));
    this->setVerticalScrollBar(new DuScrollBar(Qt::Vertical, this));
    this->setHorizontalScrollBar(new DuScrollBar(Qt::Horizontal, this));
    this->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    this->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
}

void DuListWidget::addItem(QListWidgetItem *item)
{
    QListWidget::addItem(item);
    this->updateGeometry();
    //this->updateGeometries();
}

QSize DuListWidget::sizeHint() const
{
    return QSize(200, verticalSizeHint());
}

void DuListWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_dragging)
    {
        QPoint newPos = event->globalPos();
        QPoint _delta = newPos - m_initialDragPos;
        this->horizontalScrollBar()->setValue( this->horizontalScrollBar()->value() - _delta.x() );
        this->verticalScrollBar()->setValue( this->verticalScrollBar()->value() - _delta.y() );
        m_initialDragPos = newPos;
        event->accept();
        return;
    }
    QListWidget::mouseMoveEvent(event);
}

void DuListWidget::mousePressEvent(QMouseEvent *event)
{
    // Middle click for dragging view
    if (event->button() == Qt::MiddleButton)
    {
        m_initialDragPos = event->globalPos();
        m_dragging = true;
        event->accept();
        return;
    }
    QListWidget::mousePressEvent(event);
}

void DuListWidget::mouseReleaseEvent(QMouseEvent *event)
{
    // Middle click for dragging view
    if (event->button() == Qt::MiddleButton)
    {
        m_dragging = false;
        event->accept();
        return;
    }
    QListWidget::mouseReleaseEvent(event);
}

int DuListWidget::verticalSizeHint() const
{
    return this->count() * 32;
}
