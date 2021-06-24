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
    if (m_isFilterBox)
    {
        RamObjectFilterList *proxyModel = new RamObjectFilterList(this);
        proxyModel->setList(list);
        this->setModel(proxyModel);
    }
    else
    {
        QSortFilterProxyModel *proxyModel = new QSortFilterProxyModel(this);
        proxyModel->setSourceModel(list);
        this->setModel(proxyModel);
    }
}

RamObject *RamObjectListComboBox::currentObject()
{
    quintptr iptr = this->currentData().toULongLong();
    if (iptr == 0) return nullptr;
    RamObject *obj = reinterpret_cast<RamObject*>(iptr);
    return obj;
}

QString RamObjectListComboBox::currentUuid()
{
    RamObject *obj = this->currentObject();
    if (!obj) return "";
    return obj->uuid();
}

void RamObjectListComboBox::setObject(QString uuid)
{
    if (uuid == "" && m_isFilterBox)
    {
        setCurrentIndex(0);
        return;
    }
    if (uuid == "")
    {
        setCurrentIndex(-1);
        return;
    }

    for(int i = 0; i < this->count(); i++)
    {
        quintptr iptr = this->itemData(i).toULongLong();
        RamObject *obj = reinterpret_cast<RamObject*>(iptr);
        if (!obj) continue;
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
    if (!obj) setObject("");
    else setObject(obj->uuid());
}

void RamObjectListComboBox::currentObjectChanged(int i)
{
    Q_UNUSED(i)

#ifdef DUMP_OBJECT_DEBUG
    dumpObjectInfo();
#endif

    emit currentObjectChanged( currentObject() );
    emit currentObjectChanged( currentUuid() );
}

void RamObjectListComboBox::connectEvents()
{
    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(currentObjectChanged(int)));
}
