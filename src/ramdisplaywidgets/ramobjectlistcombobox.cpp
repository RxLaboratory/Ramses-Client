#include "ramobjectlistcombobox.h"

RamObjectListComboBox::RamObjectListComboBox(QWidget *parent) :
    QComboBox(parent)
{
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
    while(!m_listConnections.isEmpty()) disconnect(m_listConnections.takeLast());

    this->setEnabled(false);

    QSignalBlocker b(this);

    m_list = list;
    if (!list) return;

    for( int i = 0; i < m_list->count(); i++) newObject(m_list->at(i));

    m_listConnections << connect(m_list, &RamObjectList::objectAdded, this, &RamObjectListComboBox::newObject);
    m_listConnections << connect(m_list, &RamObjectList::objectRemoved, this, &RamObjectListComboBox::objectRemoved);

    this->setCurrentIndex(-1);
    emit currentObjectChanged(nullptr);

    this->setEnabled(true);
}

RamObject *RamObjectListComboBox::currentObject()
{
    return m_list->fromUuid(this->currentData().toString());
}

QString RamObjectListComboBox::currentUuid()
{
    return this->currentData().toString();
}

void RamObjectListComboBox::setObject(QString uuid)
{
    for(int i = 0; i < this->count(); i++)
    {
        if (this->itemData(i).toString() == uuid)
        {
            this->setCurrentIndex(i);
            return;
        }
    }
    this->setCurrentIndex(-1);
}

void RamObjectListComboBox::setObject(RamObject *obj)
{
    setObject(obj->uuid());
}

void RamObjectListComboBox::newObject(RamObject *obj)
{
    if (!obj) return;
    QString t = obj->name();
    if (t == "") t = obj->shortName();
    this->addItem(t, obj->uuid());

    m_objectConnections[obj->uuid()] = connect(obj, &RamObject::changed, this, &RamObjectListComboBox::objectChanged);
}

void RamObjectListComboBox::objectRemoved(RamObject *obj)
{
    if (m_objectConnections.contains(obj->uuid()))
        disconnect( m_objectConnections.take(obj->uuid()));
    for (int i = 0; i < this->count(); i++)
    {
        if (this->itemData(i).toString() == obj->uuid())
        {
            this->removeItem(i);
            break;
        }
    }
}

void RamObjectListComboBox::objectChanged(RamObject *obj)
{
    for (int i = 0; i < this->count(); i++)
    {
        if (this->itemData(i).toString() == obj->uuid())
        {
            QString t = obj->name();
            if (t == "") t = obj->shortName();
            this->setItemText(i, t);
            break;
        }
    }
}

void RamObjectListComboBox::currentObjectChanged(int i)
{
    Q_UNUSED(i)
    emit currentObjectChanged( currentObject() );
}

void RamObjectListComboBox::connectEvents()
{
    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(currentObjectChanged(int)));
}
