#include "ramstatisticstablewidget.h"

RamStatisticsTableWidget::RamStatisticsTableWidget(QWidget *parent):
    QTableView(parent)
{
    setupUi();
    this->setItemDelegate( new RamStatisticsDelegate(this));
    connectEvents();
}

void RamStatisticsTableWidget::mouseMoveEvent(QMouseEvent *event)
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
    QTableView::mouseMoveEvent(event);
}

void RamStatisticsTableWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MiddleButton)
    {
        m_initialDragPos = event->globalPos();
        m_dragging = true;
        event->accept();
        return;
    }
    QTableView::mousePressEvent(event);
}

void RamStatisticsTableWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MiddleButton)
    {
        m_dragging = false;
        event->accept();
        return;
    }
    QTableView::mouseReleaseEvent(event);
}

void RamStatisticsTableWidget::showEvent(QShowEvent *event)
{
    if (!event->spontaneous())
    {
        this->resizeRowsToContents();
        this->resizeColumnsToContents();
    }
}

void RamStatisticsTableWidget::resizeEvent(QResizeEvent *event)
{
    this->setColumnWidth( 0, event->size().width() );
}

void RamStatisticsTableWidget::setupUi()
{
    this->setSelectionMode(QAbstractItemView::NoSelection);
    this->setFrameShape(QFrame::NoFrame);
    this->setDragDropMode(QAbstractItemView::NoDragDrop);
    this->setDragEnabled(false);
    this->setDefaultDropAction(Qt::IgnoreAction);
    this->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    this->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    this->verticalHeader()->setSectionsMovable(false);
    this->horizontalHeader()->setSectionsMovable(false);
    this->setShowGrid(false);
    this->setMouseTracking(true);
    this->horizontalHeader()->hide();

    QString style = "QTableView { background-color: #222222; padding-top: 3px; padding-bottom: 3px; gridline-color: rgba(0,0,0,0); selection-background-color: rgba(0,0,0,0); } ";
    style += "QTableView::item:hover { background-color: none; } ";
    style += "QHeaderView { background-color: #222222; }";
    this->setStyleSheet(style);
}

void RamStatisticsTableWidget::connectEvents()
{

}
