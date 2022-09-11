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

// STATIC //

QFrame *RamObject::ui_editWidget = nullptr;

// PUBLIC //

RamObject *RamObject::get(QString uuid, ObjectType type)
{
    Q_ASSERT(type != Object);
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

RamObject *RamObject::objectForColumn(QString columnUuid) const {
    Q_UNUSED(columnUuid);
    return nullptr;
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

RamObjectModel *RamObject::createModel(RamObject::ObjectType type, QString modelName)
{
    RamObjectModel *model = new RamObjectModel(type, this);

    // Connect all model changes to save the list
    connect(model, &RamObjectModel::rowsInserted, this, &RamObject::saveModel);
    connect(model, &RamObjectModel::rowsRemoved, this, &RamObject::saveModel);
    connect(model, &RamObjectModel::rowsMoved, this, &RamObject::saveModel);

    // keep
    m_subModels[model] = modelName;

    return model;
}

void RamObject::checkData(QString uuid)
{
    // Don't reload if we're currently saving the data
    if (m_savingData) return;
    // Not for me...
    if (uuid != m_uuid) return;
    // Empty cache!
    m_cachedData = "";
    // Reset lists
    QJsonObject d = data();
    QMapIterator<RamObjectModel *, QString> it = QMapIterator<RamObjectModel*, QString>( m_subModels );
    while (it.hasNext()) {
        it.next();
        // Clear
        RamObjectModel *model = it.key();
        QString modelName = it.value();
        loadModel(model, modelName, d);
    }
    emit dataChanged(this);
}

void RamObject::checkAvailability(QString uuid, bool availability)
{
    // Not for me...
    if (uuid != m_uuid) return;

    if (availability) emit restored(this);
    else emit removed(this);
}

void RamObject::saveModel()
{
    emit dataChanged(this);
    if (m_loadingModels) return;
    // Get the model and its name
    RamObjectModel *o = qobject_cast<RamObjectModel*>( sender() );
    QString modelName = m_subModels.value(o, "");
    if (modelName == "") return;

    // Get the stringlist
    QStringList uuids = o->toStringList();
    QJsonArray arr = QJsonArray::fromStringList(uuids);
    insertData(modelName, arr);
}

void RamObject::loadModel(RamObjectModel *model, QString modelName, QJsonObject d)
{
    if (modelName == "") return;
    m_loadingModels = true;
    if (d.isEmpty()) d = data();
    model->clear();
    // Get uuids
    QStringList uuids;
    QJsonArray arr = d.value(modelName).toArray();
    for (int i = 0; i < arr.count(); i++)
    {
        uuids << arr.at(i).toString();
    }
    // Set uuids
    model->insertObjects(0, uuids);
    m_loadingModels = false;
}

void RamObject::construct(QObject *parent)
{
    if (!parent && m_objectType != Ramses) this->setParent(Ramses::instance());
    this->setObjectName( objectTypeName() + " | " + shortName() + " (" + m_uuid + ")" );

    // Monitor db changes
    connect(LocalDataInterface::instance(), &LocalDataInterface::dataChanged, this, &RamObject::checkData);
    connect(LocalDataInterface::instance(), &LocalDataInterface::availabilityChanged, this, &RamObject::checkAvailability);
}





