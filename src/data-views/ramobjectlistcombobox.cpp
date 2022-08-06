#include "ramobjectlistcombobox.h"

#include "data-models/ramobjectfilterlist.h"
#include "ramobjectdelegate.h"

template<typename RO>
RamObjectListComboBox<RO>::RamObjectListComboBox(QWidget *parent) :
    QComboBox(parent)
{
    setupUi();
    setList(nullptr);
    connectEvents();
}

template<typename RO>
RamObjectListComboBox<RO>::RamObjectListComboBox(bool isFilterBox, QWidget *parent) :
    QComboBox(parent)
{
    setupUi();
    m_isFilterBox = isFilterBox;
    setList(nullptr);
    connectEvents();
}

template<typename RO>
RamObjectListComboBox<RO>::RamObjectListComboBox(RamObjectList<RO> *list, QWidget *parent) :
    QComboBox(parent)
{
    setupUi();
    setList(list);
    connectEvents();
}

template<typename RO>
void RamObjectListComboBox<RO>::setList(RamObjectList<RO> *list)
{
    if (m_isFilterBox)
    {
        RamObjectFilterList<RO> *proxyModel = new RamObjectFilterList<RO>(this);
        proxyModel->setList(list);
        this->setModel(proxyModel);
    }
    else
    {
        this->setModel(list);
    }
}

template<typename RO>
RO RamObjectListComboBox<RO>::currentObject()
{
    int i = this->currentIndex();
    return object(i);
}

template<typename RO>
QString RamObjectListComboBox<RO>::currentUuid()
{
    int i = this->currentIndex();
    return uuid(i);
}

template<typename RO>
void RamObjectListComboBox<RO>::setObject(QString objUuid)
{
    if (objUuid == "" && m_isFilterBox)
    {
        setCurrentIndex(0);
        return;
    }
    if (objUuid == "")
    {
        setCurrentIndex(-1);
        return;
    }

    for(int i = 0; i < this->count(); i++)
    {
        QString ou = uuid(i);
        if (objUuid == ou)
        {
            this->setCurrentIndex(i);
            return;
        }
    }
    this->setCurrentIndex(-1);
}

template<typename RO>
void RamObjectListComboBox<RO>::setObject(RO obj)
{
    if (!obj) setObject("");
    else setObject(obj->uuid());
}

template<typename RO>
RO RamObjectListComboBox<RO>::object(int i)
{
    if (m_isFilterBox)
    {
        RamObjectFilterList<RO> *m = qobject_cast<RamObjectFilterList<RO> >(model());
        return m->at(i);
    }
    else
    {
        RamObjectList<RO> *m = qobject_cast<RamObjectList<RO>>(model());
        return m->at(i);
    }
}

template<typename RO>
QString RamObjectListComboBox<RO>::uuid(int i)
{
    RO o = object(i);
    if (o) return o->uuid();
    return "";
}

template<typename RO>
void RamObjectListComboBox<RO>::beginReset()
{
    m_resetting = true;
    m_resettingObject = currentObject();
}

template<typename RO>
void RamObjectListComboBox<RO>::endReset()
{
    m_resetting = false;
    setObject(m_resettingObject);
}

template<typename RO>
void RamObjectListComboBox<RO>::showPopup()
{
    // Update size
    // get the minimum width that fits the largest item.
    int width = this->minimumSizeHint().width();
    // set the view to that width + icon and margins.
    this->view()->setMinimumWidth(width + 40);

    QComboBox::showPopup();
    emit popupShown();
}

template<typename RO>
void RamObjectListComboBox<RO>::hidePopup()
{
    QComboBox::hidePopup();
    emit popupHidden();
}

template<typename RO>
void RamObjectListComboBox<RO>::currentObjectIndexChanged(int i)
{
    Q_UNUSED(i)

#ifdef DUMP_OBJECT_DEBUG
    dumpObjectInfo();
#endif

    if (!m_resetting)
    {
        emit currentObjectChanged( currentObject() );
        emit currentUuidChanged( currentUuid() );
    }
}

template<typename RO>
void RamObjectListComboBox<RO>::objectIndexActivated(int i)
{
    Q_UNUSED(i)

#ifdef DUMP_OBJECT_DEBUG
    dumpObjectInfo();
#endif

    emit objectActivated( currentObject() );
    emit uuidActivated( currentUuid() );
}

template<typename RO>
void RamObjectListComboBox<RO>::setupUi()
{
    //this->setSizeAdjustPolicy(SizeAdjustPolicy::AdjustToContents);
    RamObjectDelegate<RO> *delegate = new RamObjectDelegate<RO>(this);
    delegate->setComboBoxMode(true);
    this->setItemDelegate(delegate);
}

template<typename RO>
void RamObjectListComboBox<RO>::connectEvents()
{
    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(currentObjectIndexChanged(int)));
}
