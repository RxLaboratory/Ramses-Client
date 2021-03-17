#include "objectnode.h"

ObjectNode::ObjectNode(RamObject *obj)
{
    _object = obj;

    // Build a dock widget to edit details
    QMainWindow *mw = GuiUtils::appMainWindow();
    _dockWidget = new ObjectDockWidget(obj, mw);

    //_dockWidget->setWidget(new ObjectEditWidget(obj, _dockWidget));
    StepEditWidget *stepWidget = new StepEditWidget(_dockWidget);
    stepWidget->setStep((RamStep*)obj);
    _dockWidget->setWidget(stepWidget);

    mw->addDockWidget(Qt::RightDockWidgetArea, _dockWidget);
    _dockWidget->hide();

    this->setTitle(_object->shortName());
    this->setTitleToolTip(_object->name());

    connect(obj, &RamStep::changed, this, &ObjectNode::objectChanged);
    connect(obj, &RamStep::removed, this, &ObjectNode::objectRemoved);
}

RamObject *ObjectNode::ramObject() const
{
    return _object;
}

QVariant ObjectNode::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemSelectedChange && scene())
        _dockWidget->setVisible(value.toBool());

    return DuQFNode::itemChange(change, value);
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
