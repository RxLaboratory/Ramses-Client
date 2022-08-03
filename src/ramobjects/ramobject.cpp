#include "ramobject.h"

#include "objecteditwidget.h"
#include "mainwindow.h"
#include "ramses.h"

// STATIC //

RamObject *RamObject::getObject(QString uuid, bool constructNew)
{
    RamAbstractObject *obj = RamAbstractObject::getObject(uuid);
    if (!obj && constructNew) return new RamObject(uuid, ObjectType::Object);
    return static_cast<RamObject*>( obj ) ;
}

// PUBLIC //

RamObject::RamObject(QString shortName, QString name, ObjectType type, QObject *parent, bool isVirtual):
    QObject(parent),
    RamAbstractObject(shortName, name, type, isVirtual)
{
    construct(parent);
}

void RamObject::remove()
{
    this->disconnect();
    RamAbstractObject::remove();
}

// PROTECTED //

RamObject::RamObject(QString uuid, ObjectType type, QObject *parent):
    QObject(parent),
    RamAbstractObject(uuid, type)
{
    construct(parent);
}

void RamObject::emitDataChanged(QJsonObject data)
{
    emit dataChanged(this, data);
}

void RamObject::emitRemoved()
{
    emit removed(this);
}

void RamObject::emitRestored()
{
    emit restored(this);
}

void RamObject::setEditWidget(ObjectEditWidget *w)
{
    ui_editWidget = new QFrame();
    QVBoxLayout *l = new QVBoxLayout();
    l->setContentsMargins(3,3,3,3);
    l->addWidget(w);
    ui_editWidget->setLayout(l);
}

void RamObject::showEdit(QString title)
{
    if (!ui_editWidget) return;

    MainWindow *mw = (MainWindow*)GuiUtils::appMainWindow();
    if (title == "") title = this->name();
    if (title == "") title = this->shortName();
    if (title == "") title = "Properties";

    ui_editWidget->setEnabled(false);

    if (m_editable)
    {
        RamUser *u = Ramses::instance()->currentUser();
        if (u)
        {
            ui_editWidget->setEnabled(u->role() >= m_editRole);
            if (u->is(this)) ui_editWidget->setEnabled(true);
        }
    }

    mw->setPropertiesDockWidget( ui_editWidget, title, m_icon);
}

void RamObject::construct(QObject *parent)
{
    this->setObjectName( objectTypeName() );
    if (!parent) setParent(Ramses::instance());
}





