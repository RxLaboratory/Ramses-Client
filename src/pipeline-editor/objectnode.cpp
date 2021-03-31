#include "objectnode.h"

ObjectNode::ObjectNode(RamObject *obj)
{
    _object = obj;

    // Build a dock widget to edit details
    QMainWindow *mw = GuiUtils::appMainWindow();
    _dockWidget = new ObjectDockWidget(obj, mw);

    mw->addDockWidget(Qt::RightDockWidgetArea, _dockWidget);
    _dockWidget->hide();

    this->setTitle(_object->shortName());
    this->setTitleToolTip(_object->name());

    connect(this, &DuQFNode::removed, this, &ObjectNode::removeObject);
    connect(obj, &RamStep::changed, this, &ObjectNode::objectChanged);
    connect(obj, &RamStep::removed, this, &ObjectNode::objectRemoved);
}

ObjectNode::~ObjectNode()
{
    _dockWidget->deleteLater();
}

RamObject *ObjectNode::ramObject() const
{
    return _object;
}

QVariant ObjectNode::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemSelectedChange && scene() && _hasEditWidget)
        _dockWidget->setVisible(value.toBool());

    return DuQFNode::itemChange(change, value);
}

void ObjectNode::setEditWidget(ObjectEditWidget *w)
{
    w->setParent(_dockWidget);
    _dockWidget->setWidget(w);
    _hasEditWidget = true;
}

void ObjectNode::removeObject()
{
    _object->remove();
}

void ObjectNode::objectChanged()
{
    this->setTitle(_object->shortName());
    this->setTitleToolTip(_object->name());
}

void ObjectNode::objectRemoved()
{
    this->remove();
}
