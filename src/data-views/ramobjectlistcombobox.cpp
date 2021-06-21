#include "ramobjectlistcombobox.h"

RamObjectListComboBox::RamObjectListComboBox(QWidget *parent) :
    QComboBox(parent)
{
    setList(nullptr);
    connectEvents();
}

RamObjectListComboBox::RamObjectListComboBox(bool isFilterBox, QWidget *parent) :
    QComboBox(parent)
{
    m_isFilterBox = isFilterBox;
    setList(nullptr);
    connectEvents();
}

RamObjectListComboBox::RamObjectListComboBox(RamObjectList *list, QWidget *parent) :
    QComboBox(parent)
{
    setList(list);
    connectEvents();
}

void RamObjectListComboBox::setList(RamObjectList *list)
{
    if (!list)
    {
        this->hide();
        return;
    }
    this->show();
    QSortFilterProxyModel *proxyModel = new QSortFilterProxyModel();
    proxyModel->setSourceModel(list);
    this->setModel(proxyModel);
}

RamObject *RamObjectListComboBox::currentObject()
{
    quintptr iptr = this->currentData().toULongLong();
    RamObject *obj = reinterpret_cast<RamObject*>(iptr);
    return obj;
}

QString RamObjectListComboBox::currentUuid()
{
    return this->currentObject()->uuid();
}

void RamObjectListComboBox::setObject(QString uuid)
{
    for(int i = 0; i < this->count(); i++)
    {
        quintptr iptr = this->itemData(i).toULongLong();
        RamObject *obj = reinterpret_cast<RamObject*>(iptr);
        if (obj->uuid() == uuid)
        {
            this->setCurrentIndex(i);
            return;
        }
    }
    this->setCurrentIndex(-1);
}

void RamObjectListComboBox::setObject(RamObject *obj)
{
    if (!obj) setCurrentIndex(-1);
    setObject(obj->uuid());
}

void RamObjectListComboBox::currentObjectChanged(int i)
{
    Q_UNUSED(i)
    emit currentObjectChanged( currentObject() );
    emit currentObjectChanged( currentUuid() );
}

void RamObjectListComboBox::connectEvents()
{
    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(currentObjectChanged(int)));
}
