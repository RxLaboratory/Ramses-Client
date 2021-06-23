#include "objectnode.h"

#include "mainwindow.h"

ObjectNode::ObjectNode(RamObject *obj):
    DuQFNode(obj->shortName())
{
    m_object = obj;

    this->setTitleToolTip(m_object->name());

    connect(obj, &RamObject::changed, this, &ObjectNode::objectChanged);
    connect(obj, &RamObject::removed, this, &ObjectNode::objectRemoved);

    this->setObjectName( "ObjectNode" );
}

RamObject *ObjectNode::ramObject() const
{
    return m_object;
}

QVariant ObjectNode::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemSelectedChange && scene())
    {
        m_object->edit( !this->isSelected() );
    }

    return DuQFNode::itemChange(change, value);
}

void ObjectNode::objectChanged()
{
    this->setTitle(m_object->shortName());
    this->setTitleToolTip(m_object->name());
}

void ObjectNode::objectRemoved()
{
    this->remove();
}

