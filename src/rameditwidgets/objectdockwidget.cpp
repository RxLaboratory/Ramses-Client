#include "objectdockwidget.h"

ObjectDockWidget::ObjectDockWidget(RamObject *obj, QWidget *parent):
    QDockWidget(obj->shortName(), parent)
{
    _object = obj;

    _titleWidget = new DuQFDockTitle(obj->name(), this);
    _titleWidget->setObjectName("dockTitle");

    this->setTitleBarWidget(_titleWidget);

    this->setToolTip(obj->name());
    this->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    this->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );

    connect(_object, &RamObject::removed, this, &ObjectDockWidget::objectRemoved);
    connect(_object, &RamObject::changed, this, &ObjectDockWidget::objectChanged);
    connect(_titleWidget, &DuQFDockTitle::pinDockWidget, this, &ObjectDockWidget::pin);
}

RamObject::ObjectType ObjectDockWidget::objectType() const
{
    if (!_object) return RamObject::Generic;
    else return _object->objectType();
}

void ObjectDockWidget::setTitle(QString title)
{
    _titleWidget->setTitle(title);
    this->setWindowTitle(title);
}

void ObjectDockWidget::setIcon(QString icon)
{
    _titleWidget->setIcon(icon);
}

void ObjectDockWidget::pin(bool p)
{
    _pinned = p;
}

void ObjectDockWidget::objectChanged(RamObject *o)
{
    this->setToolTip(o->name());
    _titleWidget->setTitle(o->name());
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
