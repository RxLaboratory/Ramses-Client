#include "ramobjectlistwidget.h"

RamObjectListWidget::RamObjectListWidget(QWidget *parent):
        QListView(parent)
{
    setupUi();
}

RamObjectListWidget::RamObjectListWidget(RamObjectList *list, QWidget *parent):
    QListView(parent)
{
    setupUi();
}

RamObjectListWidget::RamObjectListWidget(RamObjectUberList *list, QWidget *parent):
    QListView(parent)
{
    setupUi();
}

RamObjectListWidget::RamObjectListWidget(RamObjectList *list, bool editableObjects, QWidget *parent):
    QListView(parent)
{
    setupUi();
}

RamObjectListWidget::RamObjectListWidget(RamObjectUberList *list, bool editableObjects, QWidget *parent):
    QListView(parent)
{
    setupUi();
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

    RamObjectListModel *objectListModel = new RamObjectListModel(Ramses::instance()->templateSteps());
    QSortFilterProxyModel *proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(objectListModel);
    this->setModel(proxyModel);

}

