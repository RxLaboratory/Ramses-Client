#include "ramobject.h"

#include "duqf-utils/guiutils.h"
#include "objecteditwidget.h"
#include "mainwindow.h"
#include "ramapplication.h"
#include "ramasset.h"
#include "rampipe.h"
#include "rampipefile.h"
#include "ramschedulecomment.h"
#include "ramscheduleentry.h"
#include "ramsequence.h"
#include "ramses.h"

// STATIC //

RamObject *RamObject::get(QString uuid, ObjectType type)
{
    RamAbstractObject *obj = RamAbstractObject::get(uuid);
    if (obj) return static_cast<RamObject*>( obj ) ;

    // Check if the UUID exists in the database
    if (!DBInterface::instance()->hasUuid( objectTypeName(type))) return nullptr;

    switch(type)
    {
    case Application: return new RamApplication(uuid);
    case Asset: return new RamAsset(uuid);
    case AssetGroup: return new RamAssetGroup(uuid);
    case FileType: return new RamFileType(uuid);
    case Object: return new RamObject(uuid, Object);
    case Item: return new RamItem(uuid);
    case Pipe: return new RamPipe(uuid);
    case PipeFile: return new RamPipeFile(uuid);
    case Project: return new RamProject(uuid);
    case Sequence: return new RamSequence(uuid);
    case Shot: return new RamShot(uuid);
    case State: return new RamState(uuid);
    case Status: return new RamStatus(uuid);
    case Step: return new RamStep(uuid);
    case User: return new RamUser(uuid);
    case ScheduleEntry: return new RamScheduleEntry(uuid);
    case ScheduleComment: return new RamScheduleComment(uuid);
    case TemplateStep: return new RamTemplateStep(uuid);
    case TemplateAssetGroup: return new RamTemplateAssetGroup(uuid);
    case Ramses: return Ramses::instance();
    default: return new RamObject(uuid, Object);
    }
}

// PUBLIC //

RamObject::RamObject(QString shortName, QString name, ObjectType type, QObject *parent, bool isVirtual, bool encryptData):
    QObject(parent),
    RamAbstractObject(shortName, name, type, isVirtual, encryptData)
{
    construct(parent);
}

void RamObject::remove()
{
    this->disconnect();
    RamAbstractObject::remove();
}

bool RamObject::canEdit()
{
    RamUser *u = Ramses::instance()->currentUser();
    if (!u) return false;
    return u->role() >= m_editRole;
}

QIcon RamObject::icon() const
{
    return QIcon(iconName());
}

void RamObject::reload()
{
    QJsonObject d = reloadData();
    emit dataChanged(this, d );
}

// PROTECTED //

RamObject::RamObject(QString uuid, ObjectType type, QObject *parent, bool encryptData):
    QObject(parent),
    RamAbstractObject(uuid, type, encryptData)
{
    construct(parent);
}

QJsonObject RamObject::reloadData()
{
    return data();
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
    if (!parent && m_objectType != Ramses) this->setParent(Ramses::instance());
    this->setObjectName( objectTypeName() );
}





