#include "listmanagerwidget.h"

ListManagerWidget::ListManagerWidget(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);

    connect(list, SIGNAL(currentRowChanged(int)), this, SLOT(list_currentRowChanged(int)));
    connect(addButton, SIGNAL(clicked()), this, SLOT(createItem()));
    connect(removeButton, SIGNAL(clicked()), this, SLOT(remove_clicked()));
}

void ListManagerWidget::setWidget(QWidget *w)
{
    mainLayout->addWidget(w);
}

void ListManagerWidget::addItem(QListWidgetItem *item)
{
    list->addItem(item);
    list->sortItems();
}

void ListManagerWidget::list_currentRowChanged(int currentRow)
{
    currentRowChanged( currentRow );
    currentDataChanged( list->item(currentRow)->data(Qt::UserRole) );
}

void ListManagerWidget::currentRowChanged(int currentRow)
{

}

void ListManagerWidget::currentDataChanged(QVariant data)
{

}

void ListManagerWidget::createItem()
{

}

void ListManagerWidget::removeItem(QVariant data)
{

}

void ListManagerWidget::removeRow(int i)
{
    delete list->takeItem(i);
    list->sortItems();
}

void ListManagerWidget::removeData(QVariant data)
{

    for (int row = list->count() -1; row >= 0 ; row--)
    {
        if (list->item(row)->data(Qt::UserRole) == data)
        {
            removeRow(row);
        }
    }
}

void ListManagerWidget::updateItem(QVariant data, QString text)
{
    for (int row = list->count() -1; row >= 0 ; row--)
    {
        if (list->item(row)->data(Qt::UserRole) == data)
        {
            list->item(row)->setText(text);
        }
    }
}

void ListManagerWidget::remove_clicked()
{
    QListWidgetItem *i = list->currentItem();
    if (i)
    {
        removeItem(i->data(Qt::UserRole));
    }
}
