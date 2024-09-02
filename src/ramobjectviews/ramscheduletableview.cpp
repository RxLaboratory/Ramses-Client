#include "ramscheduletableview.h"
#include "qsortfilterproxymodel.h"
#include "ramschedulerowheaderview.h"

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

void RamScheduleTableView::rowMoved(int logicalIndex, int oldVisualIndex, int newVisualIndex)
{
    Q_UNUSED(logicalIndex);

    QSignalBlocker b(this->verticalHeader());

    QAbstractItemModel *m = this->model();

    // if there's a filter, get the source model
    auto fm = qobject_cast<QSortFilterProxyModel*>(m);
    if (fm)
        m = fm->sourceModel();

    m->moveRow(QModelIndex(), oldVisualIndex, QModelIndex(), newVisualIndex);
    // move back to the (new) logical index
    this->verticalHeader()->moveSection(newVisualIndex, oldVisualIndex);
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
    this->horizontalHeader()->setSectionsMovable(false);
    this->setShowGrid(false);
    //this->setMouseTracking(true);  
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    this->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked);

    m_delegate = new RamScheduleDelegate(this);
    this->setItemDelegate(m_delegate);

    this->setVerticalHeader(new RamScheduleRowHeaderView(this));
    this->verticalHeader()->setSectionsMovable(true);
}

void RamScheduleTableView::connectEvents()
{
    // SORT
    // No need to connect, the slot is overriden
    //connect( this->verticalHeader(), &QHeaderView::sectionMoved,
    //        this, &RamScheduleTableView::rowMoved);
}
