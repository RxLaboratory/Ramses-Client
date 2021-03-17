#include "objectdockwidget.h"

ObjectDockWidget::ObjectDockWidget(RamObject *obj, QWidget *parent):
    QDockWidget(obj->shortName(), parent)
{
    _object = obj;

    _titleWidget = new DuQFDockTitle(obj->shortName(), this);

    this->setTitleBarWidget(_titleWidget);

    this->setToolTip(obj->name());
    this->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    this->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable );

    connect(_object, &RamObject::removed, this, &ObjectDockWidget::objectRemoved);
    connect(_object, &RamObject::changed, this, &ObjectDockWidget::objectChanged);
    connect(_titleWidget, &DuQFDockTitle::pinDockWidget, this, &ObjectDockWidget::pin);
}

void ObjectDockWidget::pin(bool p)
{
    _pinned = p;
}

void ObjectDockWidget::objectChanged(RamObject *o)
{
    this->setToolTip(o->name());
    _titleWidget->setTitle(o->shortName());
}

void ObjectDockWidget::objectRemoved(RamObject *o)
{
    Q_UNUSED(o);
    this->deleteLater();
}

bool ObjectDockWidget::pinned() const
{
    return _pinned;
}
