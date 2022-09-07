#include "ramobjectcombobox.h"
#include "ramobjectsortfilterproxymodel.h"
#include "ramobjectdelegateold.h"

RamObjectComboBox::RamObjectComboBox(RamObject::ObjectType type, QWidget *parent):
    QComboBox(parent)
{
    setupUi();
    m_type = type;
    setList(nullptr);
    connectEvents();
}

RamObjectComboBox::RamObjectComboBox(RamObject::ObjectType type, QString filterName, QWidget *parent) :
    QComboBox(parent)
{
    setupUi();
    m_isFilterBox = true;
    m_filterName = filterName;
    m_type = type;
    setList(nullptr);
    connectEvents();
}

void RamObjectComboBox::setList(RamObjectModel *list)
{
    if (m_isFilterBox)
    {
        RamObjectSortFilterProxyModel *proxyModel = new RamObjectSortFilterProxyModel(m_filterName, this);
        proxyModel->setSourceModel(list);
        this->setModel(proxyModel);
    }
    else if (list)
    {
        this->setModel(list);
    }
    else
    {
        this->setModel( RamObjectModel::emptyModel() );
    }
}

void RamObjectComboBox::setupUi()
{
    RamObjectDelegateOld *delegate = new RamObjectDelegateOld(this);
    delegate->setComboBoxMode(true);
    this->setItemDelegate(delegate);
}

void RamObjectComboBox::connectEvents()
{
    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(currentObjectIndexChanged(int)));
    connect(this, SIGNAL(activated(int)), this, SLOT(objectIndexActivated(int)));
}

RamObject *RamObjectComboBox::currentObject()
{
    int i = this->currentIndex();
    return objectAt(i);
}

QString RamObjectComboBox::currentUuid()
{
    int i = this->currentIndex();
    return uuidAt(i);
}

void RamObjectComboBox::setObject(QString uuid)
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
        QString ou = uuidAt(i);
        if (uuid == ou)
        {
            this->setCurrentIndex(i);
            return;
        }
    }
    this->setCurrentIndex(-1);
}

void RamObjectComboBox::setObject(RamObject *obj)
{
    if (!obj) setObject("");
    else setObject(obj->uuid());
}

RamObject *RamObjectComboBox::objectAt(int i)
{
    QString uuid = uuidAt(i);
    if (uuid == "") return nullptr;
    return RamObject::get(uuid, m_type);

}

QString RamObjectComboBox::uuidAt(int i)
{
    return model()->data( model()->index(i, 0), Qt::UserRole).toString();
}

void RamObjectComboBox::beginReset()
{
    m_resetting = true;
    m_resettingUuid = currentUuid();
}

void RamObjectComboBox::endReset()
{
    m_resetting = false;
    setObject(m_resettingUuid);
}

void RamObjectComboBox::showPopup()
{
    // Update size
    // get the minimum width that fits the largest item.
    int width = this->minimumSizeHint().width();
    // set the view to that width + icon and margins.
    this->view()->setMinimumWidth(width + 40);

    QComboBox::showPopup();
    emit popupShown();
}

void RamObjectComboBox::hidePopup()
{
    QComboBox::hidePopup();
    emit popupHidden();
}

void RamObjectComboBox::currentObjectIndexChanged(int i)
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

void RamObjectComboBox::objectIndexActivated(int i)
{
    Q_UNUSED(i)

#ifdef DUMP_OBJECT_DEBUG
    dumpObjectInfo();
#endif

    emit objectActivated( currentObject() );
    emit uuidActivated( currentUuid() );
}
