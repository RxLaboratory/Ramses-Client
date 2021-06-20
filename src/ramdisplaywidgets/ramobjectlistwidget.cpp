#include "ramobjectlistwidget.h"

RamObjectListWidget::RamObjectListWidget(QWidget *parent):
        QTableView(parent)
{
    setupUi();
}

RamObjectListWidget::RamObjectListWidget(RamObjectList *list, QWidget *parent):
    QTableView(parent)
{
    setupUi();
    setList(list);
}


RamObjectListWidget::RamObjectListWidget(RamObjectList *list, bool editableObjects, QWidget *parent):
    QTableView(parent)
{
    setupUi();
    setList(list);
}

void RamObjectListWidget::setList(RamObjectList *list)
{
    this->setModel(list);
}


void RamObjectListWidget::setupUi()
{
    this->setSelectionMode(QAbstractItemView::ExtendedSelection);
    this->setFrameShape(QFrame::NoFrame);
    this->setDragDropMode(QAbstractItemView::NoDragDrop);
    this->setDragEnabled(false);
    this->setDefaultDropAction(Qt::IgnoreAction);
    this->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

    RamObjectItemDelegate *delegate = new RamObjectItemDelegate();
    this->setItemDelegate( delegate );
}

