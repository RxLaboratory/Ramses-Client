#include "ramobjectlistcombobox.h"

RamObjectListComboBox::RamObjectListComboBox(QWidget *parent) :
    QComboBox(parent)
{
    setupUi();
    setList(nullptr);
    connectEvents();
}

RamObjectListComboBox::RamObjectListComboBox(bool isFilterBox, QWidget *parent) :
    QComboBox(parent)
{
    setupUi();
    m_isFilterBox = isFilterBox;
    setList(nullptr);
    connectEvents();
}

RamObjectListComboBox::RamObjectListComboBox(RamObjectList *list, QWidget *parent) :
    QComboBox(parent)
{
    setupUi();
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

void RamObjectListComboBox::showPopup()
{
    // Update size
    // get the minimum width that fits the largest item.
    int width = this->minimumSizeHint().width();
    // set the view to that width + icon and margins.
    this->view()->setMinimumWidth(width + 40);

    QComboBox::showPopup();
    emit popupShown();
}

void RamObjectListComboBox::hidePopup()
{
    QComboBox::hidePopup();
    emit popupHidden();
}

void RamObjectListComboBox::currentObjectIndexChanged(int i)
{
    Q_UNUSED(i)

#ifdef DUMP_OBJECT_DEBUG
    dumpObjectInfo();
#endif

    emit currentObjectChanged( currentObject() );
    emit currentUuidChanged( currentUuid() );
}

void RamObjectListComboBox::objectIndexActivated(int i)
{
    Q_UNUSED(i)

#ifdef DUMP_OBJECT_DEBUG
    dumpObjectInfo();
#endif

    emit objectActivated( currentObject() );
    emit uuidActivated( currentUuid() );
}

void RamObjectListComboBox::setupUi()
{
    //this->setSizeAdjustPolicy(SizeAdjustPolicy::AdjustToContents);
    RamObjectDelegate *delegate = new RamObjectDelegate(this);
    delegate->setComboBoxMode(true);
    this->setItemDelegate(delegate);
}

void RamObjectListComboBox::connectEvents()
{
    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(currentObjectIndexChanged(int)));
}
