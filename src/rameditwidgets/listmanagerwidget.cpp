#include "listmanagerwidget.h"

ListManagerWidget::ListManagerWidget(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);

    filterWidget->hide();
    list->setDragEnabled(false);
    list->setSelectionMode(QAbstractItemView::SingleSelection);

    _role = RamUser::Standard;

    connect(list, SIGNAL(currentRowChanged(int)), this, SLOT(list_currentRowChanged(int)));
    connect(addButton, SIGNAL(clicked()), this, SLOT(createItem()));
    connect(removeButton, SIGNAL(clicked()), this, SLOT(remove_clicked()));
    connect(Ramses::instance(), &Ramses::loggedIn, this, &ListManagerWidget::loggedIn);
    connect(Ramses::instance(), &Ramses::loggedOut, this, &ListManagerWidget::loggedOut);
    connect(filterBox, SIGNAL(currentIndexChanged(int)), this, SLOT(filterIndexChanged(int)));

    this->setEnabled(false);

    splitter->setSizes(QList<int>() << 20 << 100);
}

void ListManagerWidget::setWidget(QWidget *w)
{
    mainLayout->addStretch(0);
    mainLayout->addWidget(w);
    mainLayout->addStretch(0);
    mainLayout->setStretch(0, 0);
    mainLayout->setStretch(1, 20);
    mainLayout->setStretch(2, 80);
    mainLayout->setStretch(3, 20);
}

void ListManagerWidget::addFilter(QString name, QString data)
{
    filterBox->addItem(name, data);
    filterWidget->show();
}

void ListManagerWidget::updateFilterName(QString name, QString data)
{
    for(int i =0; i < filterBox->count(); i++)
    {
        if (filterBox->itemData(i).toString() == data) filterBox->setItemText(i, name);
    }
}

QString ListManagerWidget::currentFilter()
{
    return filterBox->currentData().toString();
}

void ListManagerWidget::clearFilters()
{
    filterBox->clear();
    filterWidget->hide();
}

void ListManagerWidget::removeFilter(QString data)
{
    for(int i =0; i < filterBox->count(); i++)
    {
        if (filterBox->itemData(i).toString() == data) filterBox->removeItem(i);
    }
    if (filterBox->count() == 0) filterWidget->hide();
}

void ListManagerWidget::addItem(QListWidgetItem *item)
{
    list->addItem(item);
    QVariant currentData = item->data(Qt::UserRole);
    //list->sortItems();
    //reselect
    list->setCurrentItem(item, QItemSelectionModel::ClearAndSelect);
}

void ListManagerWidget::insertItem(int index, QListWidgetItem *item)
{
    list->insertItem(index, item);
    list->setCurrentItem(item, QItemSelectionModel::ClearAndSelect);
}

void ListManagerWidget::list_currentRowChanged(int currentRow)
{
    currentRowChanged( currentRow );
    currentDataChanged( list->item(currentRow)->data(Qt::UserRole) );
}

void ListManagerWidget::currentRowChanged(int currentRow)
{
    Q_UNUSED(currentRow);
}

void ListManagerWidget::currentDataChanged(QVariant data)
{
    Q_UNUSED(data);
}

void ListManagerWidget::createItem()
{

}

void ListManagerWidget::removeItem(QVariant data)
{
    Q_UNUSED(data);
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

void ListManagerWidget::setRole(RamUser::UserRole r)
{
    _role = r;
}

void ListManagerWidget::remove_clicked()
{
    QListWidgetItem *i = list->currentItem();
    if (i)
    {
        QMessageBox::StandardButton confirm = QMessageBox::question(this,
                                                                    "Confirm deletion",
                                                                    "Are you sure you want to premanently remove " + i->text() + "?" );
        if (confirm == QMessageBox::Yes)
        {
            removeItem(i->data(Qt::UserRole));
        }
    }
}

void ListManagerWidget::loggedIn(RamUser *user)
{
    this->setEnabled(false);
    if (!user) return;
    this->setEnabled(user->role() >= _role);
}

void ListManagerWidget::loggedOut()
{
    this->setEnabled(false);
}

void ListManagerWidget::filterIndexChanged(int i)
{
    emit filterChanged( filterBox->itemData(i).toString() );
}

void ListManagerWidget::selectRow(int r)
{
    for (int i = 0; i < list->count(); i++)
    {
        list->item(i)->setSelected(i == r);
    }
}
