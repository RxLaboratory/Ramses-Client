#include "ramobject.h"

#include "duqf-utils/guiutils.h"
#include "objecteditwidget.h"
#include "mainwindow.h"
#include "ramapplication.h"
#include "ramasset.h"
#include "rampipe.h"
#include "rampipefile.h"
#include "ramsequence.h"
#include "ramscheduleentry.h"
#include "ramschedulecomment.h"
#include "ramses.h"
#include "ramshot.h"

// PUBLIC //

RamObject *RamObject::get(QString uuid, ObjectType type)
{
    Q_ASSERT(type != Object);
    Q_ASSERT(type != ObjectList);
    Q_ASSERT(type != ItemTable);
    Q_ASSERT(type != StepStatusHistory);
    Q_ASSERT(type != Item);

    switch(type)
    {
    case Application: return RamApplication::get(uuid);
    case Asset: return RamAsset::get(uuid);
    case AssetGroup: return RamAssetGroup::get(uuid);
    case FileType: return RamFileType::get(uuid);
    case Pipe: return RamPipe::get(uuid);
    case PipeFile: return RamPipeFile::get(uuid);
    case Project: return RamProject::get(uuid);
    case Sequence: return RamSequence::get(uuid);
    case Shot: return RamShot::get(uuid);
    case State: return RamState::get(uuid);
    case Status: return RamStatus::get(uuid);
    case Step: return RamStep::get(uuid);
    case User: return RamUser::get(uuid);
    case ScheduleEntry: return RamScheduleEntry::get(uuid);
    case ScheduleComment: return RamScheduleComment::get(uuid);
    case TemplateStep: return RamTemplateStep::get(uuid);
    case TemplateAssetGroup: return RamTemplateAssetGroup::get(uuid);
    case Ramses: return Ramses::instance();
        // These aren't valid RamObjects
    case ObjectList: return nullptr;
    case ItemTable: return nullptr;
    case StepStatusHistory: return nullptr;
    case Item: return nullptr;
    case Object: return nullptr;
    }

    return nullptr;
}

RamObject::RamObject(QString shortName, QString name, ObjectType type, QObject *parent, bool isVirtual, bool encryptData):
    QObject(parent),
    RamAbstractObject(shortName, name, type, isVirtual, encryptData)
{
    construct(parent);
}

void RamObject::remove()
{
    RamAbstractObject::remove();
    this->disconnect();
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
    emit dataChanged(this);
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

void RamObject::emitDataChanged()
{
    emit dataChanged(this);
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
    this->setObjectName( objectTypeName() + " | " + shortName() + " (" + m_uuid + ")" );
}





