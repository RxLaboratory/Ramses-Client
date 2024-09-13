#include "ramobject.h"

#include "duutils/guiutils.h"
#include "objecteditwidget.h"
#include "mainwindow.h"
#include "ramapplication.h"
#include "ramasset.h"
#include "rampipe.h"
#include "rampipefile.h"
#include "ramsequence.h"
#include "ramscheduleentry.h"
#include "ramses.h"
#include "ramshot.h"
#include "ramstatus.h"
#include "ramjsonobjecteditwidget.h"

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
    case ScheduleRow: return RamScheduleRow::get(uuid);
    case TemplateStep: return RamTemplateStep::get(uuid);
    case TemplateAssetGroup: return RamTemplateAssetGroup::get(uuid);
    case Ramses: return Ramses::i();
        // These aren't valid RamObjects
    case Item: return nullptr;
    case Object: return nullptr;
    }

    return nullptr;
}

RamObject *RamObject::get(QString uuid, QString tableName)
{
    return RamObject::get(uuid, RamObject::objectTypeFromName(tableName));
}

bool RamObject::validateData(const QString &data, ObjectType type)
{
    switch(type) {
    case Application:
        return RamApplication::validateData(data);
    case Asset:
        return RamAsset::validateData(data);
    case AssetGroup:
        return RamAssetGroup::validateData(data);
    case FileType:
        return RamFileType::validateData(data);
    case Pipe:
        return RamPipe::validateData(data);
    case PipeFile:
        return RamPipeFile::validateData(data);
    case Project:
        return RamProject::validateData(data);
    case Sequence:
        return RamSequence::validateData(data);
    case Shot:
        return RamShot::validateData(data);
    case State:
        return RamState::validateData(data);
    case Status:
        return RamStatus::validateData(data);
    case Step:
        return RamStep::validateData(data);
    case User:
        return RamUser::validateData(data);
    case ScheduleEntry:
        return RamScheduleEntry::validateData(data);
    case ScheduleRow:
        return RamScheduleRow::validateData(data);
    case TemplateStep:
        return RamTemplateStep::validateData(data);
    case TemplateAssetGroup:
        return RamTemplateAssetGroup::validateData(data);
    case Ramses:
    case Object:
    case Item:
        break;
    }
    return true;
}

RamObject::RamObject(QString shortName, QString name, ObjectType type, QObject *parent, bool isVirtual):
    QObject(parent),
    RamAbstractObject(shortName, name, type, isVirtual)
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
    RamUser *u = Ramses::i()->currentUser();
    if (!u) return false;
    return u->role() >= m_editRole;
}

QJsonObject RamObject::toJson() const
{
    QJsonObject obj;

    obj.insert(RamAbstractObject::KEY_Name, name() );
    obj.insert(RamAbstractObject::KEY_ShortName, shortName());
    obj.insert(RamAbstractObject::KEY_Comment, comment());
    obj.insert(RamAbstractObject::KEY_Color, color().name());

    obj.insert(RamAbstractObject::KEY_CustomSettings, customSettings() );

    obj.insert(RamAbstractObject::KEY_Uuid, m_uuid );

    return obj;
}

void RamObject::loadJson(const QJsonObject &obj)
{
    beginLoadJson(obj);
    endLoadJson();
}

void RamObject::reload()
{
    QJsonObject d = reloadData();
    emit dataChanged(this);
}

// PROTECTED //

RamObject::RamObject(QString uuid, ObjectType type, QObject *parent):
    QObject(parent),
    RamAbstractObject(uuid, type)
{
    construct(parent);
}

QJsonObject RamObject::reloadData()
{
    return data();
}

void RamObject::beginLoadJson(const QJsonObject &obj)
{
    blockSignals(true);

    setName(obj.value(RamAbstractObject::KEY_Name).toString());
    setShortName(obj.value(RamAbstractObject::KEY_ShortName).toString());
    setComment(obj.value(RamAbstractObject::KEY_Comment).toString());
    setColor(QColor(
        obj.value(RamAbstractObject::KEY_Color).toString(RamAbstractObject::DEFAULT_Color)
        ));

    setCustomSettings(obj.value(RamAbstractObject::KEY_CustomSettings).toString( ));
}

void RamObject::endLoadJson()
{
    blockSignals(false);
    _blockEditorUpdate = true;
    emitDataChanged();
    _blockEditorUpdate = false;
}

void RamObject::emitDataChanged()
{
    if (!m_saveSuspended) emit dataChanged(this);
}

void RamObject::emitRemoved()
{
    emit removed(this);
}

void RamObject::emitRestored()
{
    emit restored(this);
}

QFrame *RamObject::createEditFrame(QWidget *w)
{
    QFrame *f = new QFrame();
    QVBoxLayout *l = new QVBoxLayout();
    l->setContentsMargins(3,3,3,3);
    l->addWidget(w);
    f->setLayout(l);
    return f;
}

void RamObject::showEdit(QWidget *w, QString title)
{
    if (!w) return;

    auto jsonEdit = qobject_cast<RamJsonObjectEditWidget*>( w );
    if (jsonEdit) {
        ui_currentJsonEditor = jsonEdit;
        updateJsonEditor();
    }
    // Deprecated
    else {
        ObjectEditWidget *oEdit = qobject_cast<ObjectEditWidget*>( w->layout()->itemAt(0)->widget() );
        if (oEdit) oEdit->setObject(this);
    }

    MainWindow *mw = (MainWindow*)GuiUtils::appMainWindow();
    if (title == "") title = this->name();
    if (title == "") title = this->shortName();
    if (title == "") title = "Properties";

    w->setEnabled(false);

    if (m_editable)
    {
        RamUser *u = Ramses::i()->currentUser();
        if (u)
        {
            w->setEnabled(u->role() >= m_editRole);
            if (u->is(this)) w->setEnabled(true);
        }
    }

    mw->setPropertiesDockWidget( w, title, m_icon);
}

void RamObject::updateJsonEditor()
{
    if (!ui_currentJsonEditor ||
        _blockEditorUpdate)
        return;

    ui_currentJsonEditor->setData(toJson(), m_uuid);
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

void RamObject::checkData(QString uuid, QString d, QString table)
{
    // Don't reload if we're currently saving the data
    if (m_savingData) return;
    // Not for me...
    if (table != objectTypeName()) return;
    if (uuid != m_uuid) return;
    // Update cache!
    m_cachedData = d;
    // Reset lists
    QJsonObject dataObj = data();
    QMapIterator<RamObjectModel *, QString> it = QMapIterator<RamObjectModel*, QString>( m_subModels );
    while (it.hasNext()) {
        it.next();
        // Clear
        RamObjectModel *model = it.key();
        QString modelName = it.value();
        //if (m_objectType != RamAbstractObject::Project)
            loadModel(model, modelName, dataObj);
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
    QVector<QString> uuids;
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
    if (!parent && m_objectType != Ramses) this->setParent(Ramses::i());
    this->setObjectName( objectTypeName() + " | " + shortName() + " (" + m_uuid + ")" );

    // Update editor on data change
    connect(this, &RamObject::dataChanged, this, &RamObject::updateJsonEditor);

    // Monitor db changes
    connect(LocalDataInterface::instance(), &LocalDataInterface::dataChanged, this, &RamObject::checkData);
    connect(LocalDataInterface::instance(), &LocalDataInterface::availabilityChanged, this, &RamObject::checkAvailability);
}





