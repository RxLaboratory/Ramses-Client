#include "statisticsview.h"

#include "statisticsdelegate.h"

StatisticsView::StatisticsView(QWidget *parent):
    DuTableView(parent)
{
    setupUi();
    this->setItemDelegate( new StatisticsDelegate(this));
    connectEvents();
    m_statsModel = new StatisticsModel(this);
    this->setModel(m_statsModel);
}

void StatisticsView::setUser(RamUser *user)
{
    m_statsModel->setUser(user);
}

void StatisticsView::showEvent(QShowEvent *event)
{
    if (!event->spontaneous())
    {
        this->resizeRowsToContents();
        this->resizeColumnsToContents();
    }
}

void StatisticsView::resizeEvent(QResizeEvent *event)
{
    this->setColumnWidth( 0, event->size().width() );
}

void StatisticsView::setupUi()
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

    this->horizontalHeader()->setStretchLastSection(true);
}

void StatisticsView::connectEvents()
{

}
