#include "dutreewidget.h"
#include "duqf-app/duui.h"
#include "duqf-widgets/duscrollbar.h"

DuTreeWidget::DuTreeWidget(QWidget *parent):
    QTreeWidget (parent)
{
    this->setFrameStyle(QFrame::NoFrame);
    this->setSelectionMode(QAbstractItemView::ExtendedSelection);
    this->setStyleSheet(DuUI::css("DuTreeView"));
    this->setVerticalScrollBar(new DuScrollBar(Qt::Vertical, this));
    this->setHorizontalScrollBar(new DuScrollBar(Qt::Horizontal, this));
    this->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    this->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
}

void DuTreeWidget::addTopLevelItem(QTreeWidgetItem *item)
{
    QTreeWidget::addTopLevelItem(item);
    this->updateGeometry();
    //this->updateGeometries();
}

QSize DuTreeWidget::sizeHint() const
{
    return QSize(200, verticalSizeHint());
}

void DuTreeWidget::mouseMoveEvent(QMouseEvent *event)
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
    QTreeWidget::mouseMoveEvent(event);
}

void DuTreeWidget::mousePressEvent(QMouseEvent *event)
{
    // Middle click for dragging view
    if (event->button() == Qt::MiddleButton)
    {
        m_initialDragPos = event->globalPos();
        m_dragging = true;
        event->accept();
        return;
    }
    QTreeWidget::mousePressEvent(event);
}

void DuTreeWidget::mouseReleaseEvent(QMouseEvent *event)
{
    // Middle click for dragging view
    if (event->button() == Qt::MiddleButton)
    {
        m_dragging = false;
        event->accept();
        return;
    }
    QTreeWidget::mouseReleaseEvent(event);
}

int DuTreeWidget::verticalSizeHint() const
{
    return this->topLevelItemCount() * 32;
}
