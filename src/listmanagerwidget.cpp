#include "listmanagerwidget.h"

ListManagerWidget::ListManagerWidget(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);

    splitter->setSizes(QList<int>() << 20 << 100);

    _role = RamUser::Standard;

    connect(list, SIGNAL(currentRowChanged(int)), this, SLOT(list_currentRowChanged(int)));
    connect(addButton, SIGNAL(clicked()), this, SLOT(createItem()));
    connect(removeButton, SIGNAL(clicked()), this, SLOT(remove_clicked()));
    connect(Ramses::instance(), &Ramses::loggedIn, this, &ListManagerWidget::loggedIn);
    connect(Ramses::instance(), &Ramses::loggedOut, this, &ListManagerWidget::loggedOut);

    this->setEnabled(false);
}

void ListManagerWidget::setWidget(QWidget *w)
{
    mainLayout->addWidget(w);
}

void ListManagerWidget::addItem(QListWidgetItem *item)
{
    list->addItem(item);
    QVariant currentData = item->data(Qt::UserRole);
    list->sortItems();
    //reselect
    for (int row = 0; row < list->count(); row++)
    {
        if (list->item(row)->data(Qt::UserRole) == currentData)
        {
            list->setCurrentRow(row);
            return;
        }
    }
}

void ListManagerWidget::insertItem(int index, QListWidgetItem *item)
{
    list->insertItem(index, item);
    list->setCurrentRow(index);
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
