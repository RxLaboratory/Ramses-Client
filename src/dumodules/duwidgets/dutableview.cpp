#include "dutableview.h"
#include "duwidgets/duscrollbar.h"
#include "duapp/duui.h"
#include "qevent.h"
#include "qheaderview.h"

DuTableView::DuTableView(QWidget *parent):
    QTableView(parent)
{
    this->horizontalHeader()->setStretchLastSection(true);
    this->setFrameStyle(QFrame::NoFrame);
    this->setShowGrid(false);
    this->setStyleSheet(DuUI::css("DuTableView"));
    this->setVerticalScrollBar(new DuScrollBar(Qt::Vertical, this));
    this->setHorizontalScrollBar(new DuScrollBar(Qt::Horizontal, this));
    this->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    this->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
}

void DuTableView::setVerticalHeaderMoveData()
{
    connect(this->verticalHeader(), &QHeaderView::sectionMoved,
            this, &DuTableView::moveRowData);
    connect(this->verticalHeader(), &QHeaderView::sectionPressed, this, [this]() {
        m_vheaderState = this->verticalHeader()->saveState();
    });
}

void DuTableView::setHorizontalHeaderMoveData()
{
    connect(this->horizontalHeader(), &QHeaderView::sectionMoved,
            this, &DuTableView::moveColumnData);
    connect(this->horizontalHeader(), &QHeaderView::sectionPressed, this, [this]() {
        m_hheaderState = this->horizontalHeader()->saveState();
    });
}

void DuTableView::mouseMoveEvent(QMouseEvent *event)
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

void DuTableView::mousePressEvent(QMouseEvent *event)
{
    // Middle click for dragging view
    if (event->button() == Qt::MiddleButton)
    {
        m_initialDragPos = event->globalPos();
        m_dragging = true;
        event->accept();
        return;
    }
    QTableView::mousePressEvent(event);
}

void DuTableView::mouseReleaseEvent(QMouseEvent *event)
{
    // Middle click for dragging view
    if (event->button() == Qt::MiddleButton)
    {
        m_dragging = false;
        event->accept();
        return;
    }
    QTableView::mouseReleaseEvent(event);
}

void DuTableView::moveRowData(int logicalIndex, int oldVisualIndex, int newVisualIndex)
{
    Q_UNUSED(oldVisualIndex);
    // Restore the old header state
    this->verticalHeader()->restoreState(m_vheaderState);
    // Move in the model
    this->model()->moveRow(QModelIndex(), logicalIndex, QModelIndex(), newVisualIndex);
}


void DuTableView::moveColumnData(int logicalIndex, int oldVisualIndex, int newVisualIndex)
{
    Q_UNUSED(oldVisualIndex);
    // Restore the old header state
    this->horizontalHeader()->restoreState(m_hheaderState);
    // Move in the model
    this->model()->moveColumn(QModelIndex(), logicalIndex, QModelIndex(), newVisualIndex);
}
