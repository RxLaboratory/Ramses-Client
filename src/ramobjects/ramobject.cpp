#include "ramobject.h"
#include "objectdockwidget.h"
#include "mainwindow.h"

QMap<QString, RamObject*> RamObject::m_existingObjects = QMap<QString, RamObject*>();

RamObject::RamObject(QObject *parent) : QObject(parent)
{
    m_removing = false;
    m_shortName = "";
    m_name = "";
    m_uuid = RamUuid::generateUuidString(m_shortName);
    m_dbi = DBInterface::instance();
    this->setObjectName( "RamObject" );
    m_existingObjects[m_uuid] = this;
}

RamObject::RamObject(QString uuid, QObject *parent): QObject(parent)
{
    m_removing = false;
    m_shortName = "";
    m_name = "";
    m_uuid = uuid;
    m_dbi = DBInterface::instance();
    this->setObjectName( "RamObject" );
    m_existingObjects[m_uuid] = this;
}

RamObject::RamObject(QString shortName, QString name, QString uuid, QObject *parent) : QObject(parent)
{
    m_removing = false;
    m_shortName = shortName;
    m_name = name;
    if(m_name == "" ) m_name = shortName;
    if (uuid != "") m_uuid = uuid;
    else m_uuid = RamUuid::generateUuidString(m_shortName);
    m_dbi = DBInterface::instance();
    this->setObjectName( "RamObject" );
    m_existingObjects[m_uuid] = this;
}

QString RamObject::shortName() const
{
    return m_shortName;
}

void RamObject::setShortName(const QString &shortName)
{
    if (shortName == m_shortName) return;
    m_dirty  = true;
    m_shortName = shortName;
    emit changed(this);
}

QString RamObject::name() const
{
    return m_name;
}

void RamObject::setName(const QString &name)
{
    if (name == m_name) return;
    m_dirty = true;
    m_name = name;
    emit changed(this);
}

QString RamObject::comment() const
{
    return m_comment;
}

void RamObject::setComment(const QString comment)
{
    if (comment == m_comment) return;
    m_dirty = true;
    m_comment = comment;
    emit changed(this);
}

QString RamObject::uuid() const
{
    return m_uuid;
}

void RamObject::remove()
{
    qDebug().noquote() << "Removing: " + m_name + " (uuid: " + m_uuid + ")";
    qDebug().noquote() << "- " + this->objectName();
    if (m_removing) return;
    qDebug().noquote() << "> Accepted";

    m_removing = true;
#ifdef DUMP_OBJECT_DEBUG
    dumpObjectInfo();
#endif
    emit removed(this);
    qDebug().noquote() << "> " + m_name + " Removed";

    this->deleteLater();
}

void RamObject::setEditWidget(QWidget *w)
{
    m_dockWidget = new ObjectDockWidget(this);
    QFrame *f = new QFrame(m_dockWidget);
    QVBoxLayout *l = new QVBoxLayout();
    l->setContentsMargins(3,3,3,3);
    l->addWidget(w);
    f->setLayout(l);
    m_dockWidget->setWidget(f);

    MainWindow *mw = (MainWindow*)GuiUtils::appMainWindow();
    mw->addObjectDockWidget(m_dockWidget);
}

void RamObject::showEdit()
{
    if (m_dockWidget != nullptr )
        m_dockWidget->show();
}

RamObject::ObjectType RamObject::objectType() const
{
    return _objectType;
}

void RamObject::setObjectType(RamObject::ObjectType type)
{
    _objectType = type;
}

int RamObject::order() const
{
    return m_order;
}

void RamObject::setOrder(int order)
{
    if (order == m_order) return;
    m_dirty = true;
    int previous = m_order;
    m_order = order;
    if (!m_dbi->isSuspended()) m_orderChanged = true;
    emit orderChanged(this, previous, order);
    emit orderChanged();
}

QString RamObject::filterUuid() const
{
    return m_filterUuid;
}

bool RamObject::is(const RamObject *other)
{
    if (!other) return false;
    return other->uuid() == m_uuid;
}

RamObject *RamObject::obj(QString uuid)
{
    RamObject *obj = m_existingObjects.value(uuid, nullptr );
    if (!obj) obj = new RamObject(uuid);
    return obj;
}

void RamObject::update()
{
    m_dirty = false;
}
