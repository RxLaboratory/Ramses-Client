#include "ramscheduletableview.h"

RamScheduleTableView::RamScheduleTableView(QWidget *parent):
    DuTableView(parent)
{
    setupUi();
    connectEvents();
}

void RamScheduleTableView::showDetails(bool s)
{
    m_delegate->showDetails(s);
    this->resizeRowsToContents();
    this->resizeColumnsToContents();
}

void RamScheduleTableView::setupUi()
{
    this->setSelectionMode(QAbstractItemView::ExtendedSelection);
    this->setFrameShape(QFrame::NoFrame);
    this->setDragDropMode(QAbstractItemView::NoDragDrop);
    this->setDragEnabled(false);
    this->setDefaultDropAction(Qt::IgnoreAction);
    this->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    this->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    this->verticalHeader()->setSectionsMovable(false);
    // For some reason, the vertical header width is too low
    this->verticalHeader()->setMinimumWidth(150);
    this->horizontalHeader()->setSectionsMovable(false);
    this->setShowGrid(false);
    //this->setMouseTracking(true);

    this->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked);

    m_delegate = new RamScheduleDelegate(this);
    this->setItemDelegate(m_delegate);
}

void RamScheduleTableView::connectEvents()
{

}
