#include "objectnode.h"

#include "mainwindow.h"

ObjectNode::ObjectNode(RamObject *obj):
    DuQFNode(obj->shortName())
{
    _object = obj;

    // Build a dock widget to edit details
    _dockWidget = new ObjectDockWidget(obj);

    _dockWidget->hide();

    this->setTitleToolTip(_object->name());

    connect(obj, &RamObject::changed, this, &ObjectNode::objectChanged);
    connect(obj, &RamObject::removed, this, &ObjectNode::objectRemoved);

    this->setObjectName( "ObjectNode" );
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
        _dockWidget->setVisible(value.toBool() || _dockWidget->pinned());

    return DuQFNode::itemChange(change, value);
}

void ObjectNode::setEditWidget(ObjectEditWidget *w)
{
    w->setParent(_dockWidget);
    _dockWidget->setWidget(w);
    _hasEditWidget = true;
    MainWindow *mw = (MainWindow*)GuiUtils::appMainWindow();
    mw->addObjectDockWidget(_dockWidget);
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

ObjectDockWidget *ObjectNode::dockWidget() const
{
    return _dockWidget;
}
