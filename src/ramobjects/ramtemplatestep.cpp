#include "ramtemplatestep.h"

#include "templatestepeditwidget.h"

// KEYS //

const QString RamTemplateStep::KEY_StepType = QStringLiteral("type");
const QString RamTemplateStep::KEY_PublishSettings = QStringLiteral("publishSettings");
const QString RamTemplateStep::KEY_EstimationMethod = QStringLiteral("shortName");
const QString RamTemplateStep::KEY_EstimationVeryEasy = QStringLiteral("estimationVeryEasy");
const QString RamTemplateStep::KEY_EstimationEasy = QStringLiteral("estimationEasy");
const QString RamTemplateStep::KEY_EstimationMedium = QStringLiteral("estimationMedium");
const QString RamTemplateStep::KEY_EstimationHard = QStringLiteral("estimationHard");
const QString RamTemplateStep::KEY_EstimationVeryHard = QStringLiteral("estimationVeryHard");

const QString RamTemplateStep::ENUMVALUE_PreProd = QStringLiteral("pre");
const QString RamTemplateStep::ENUMVALUE_Shot = QStringLiteral("shot");
const QString RamTemplateStep::ENUMVALUE_Asset = QStringLiteral("asset");
const QString RamTemplateStep::ENUMVALUE_PostProd = QStringLiteral("post");
const QString RamTemplateStep::ENUMVALUE_Second = QStringLiteral("second");

// STATIC //

QFrame *RamTemplateStep::ui_editWidget = nullptr;

QHash<QString, RamTemplateStep*> RamTemplateStep::m_existingObjects = QHash<QString, RamTemplateStep*>();

QMetaEnum const RamTemplateStep::m_stepTypeMeta = QMetaEnum::fromType<RamTemplateStep::Type>();

const QString RamTemplateStep::stepTypeName(Type type)
{
    return QString(m_stepTypeMeta.valueToKey(type));
}

RamTemplateStep::Type RamTemplateStep::stepTypeFromName(QString typeName)
{
    if (typeName == "PreProduction") return PreProduction;
    if (typeName == "AssetProduction") return AssetProduction;
    if (typeName == "ShotProduction") return ShotProduction;
    if (typeName == "PostProduction") return PostProduction;
    return PreProduction;
}

RamTemplateStep *RamTemplateStep::get(QString uuid)
{
    if (!checkUuid(uuid, TemplateStep)) return nullptr;

    RamTemplateStep *t = m_existingObjects.value(uuid);
    if (t) return t;

    // Finally return a new instance
    return new RamTemplateStep(uuid);
}

RamTemplateStep *RamTemplateStep::c(RamObject *o)
{
    //return qobject_cast<RamTemplateStep*>(o);
    // For performance, reinterpret_cast, but be careful with the object passed!
    return reinterpret_cast<RamTemplateStep*>(o);
}

// PUBLIC //

RamTemplateStep::RamTemplateStep(QString shortName, QString name, ObjectType type) :
    RamObject(shortName, name, type)
{
    construct();

    // Preprod as default step
    setType("pre");

    createData();
}

RamTemplateStep::RamTemplateStep(QString uuid, ObjectType type):
    RamObject(uuid, type)
{
    construct();

    loadModel(m_applications, "applications");
}

RamObjectModel *RamTemplateStep::applications() const
{
    return m_applications;
}

RamTemplateStep::Type RamTemplateStep::type() const
{
    QString typeStr = getData(KEY_StepType).toString();

    if (typeStr == ENUMVALUE_Asset) return AssetProduction;
    else if (typeStr == ENUMVALUE_Shot) return ShotProduction;
    else if (typeStr == ENUMVALUE_PostProd) return PostProduction;
    else return PreProduction;
}

void RamTemplateStep::setType(const Type &type)
{
    switch(type){
    case PreProduction:
        insertData(KEY_StepType, ENUMVALUE_PreProd);
        break;
    case AssetProduction:
        insertData(KEY_StepType, ENUMVALUE_Asset);
        break;
    case ShotProduction:
        insertData(KEY_StepType, ENUMVALUE_Shot);
        break;
    case PostProduction:
        insertData(KEY_StepType, ENUMVALUE_PostProd);
        break;
    }
}

void RamTemplateStep::setType(QString type)
{
    insertData(KEY_StepType, type);
}

QString RamTemplateStep::publishSettings() const
{
    return getData(KEY_PublishSettings).toString();
}

void RamTemplateStep::setPublishSettings(const QString &newPublishSettings)
{
    insertData(KEY_PublishSettings, newPublishSettings);
}

float RamTemplateStep::estimationVeryHard() const
{
    return getData(KEY_EstimationVeryHard).toDouble(4);
}

void RamTemplateStep::setEstimationVeryHard(float newEstimationVeryHard)
{
    insertData(KEY_EstimationVeryHard, newEstimationVeryHard);
}

QString RamTemplateStep::iconName() const
{
    switch(type())
    {
    case PreProduction:
        return ":/icons/project";
    case ShotProduction:
        return ":/icons/shot";
    case AssetProduction:
        return ":/icons/asset";
    case PostProduction:
        return ":/icons/film";
    }
    return ":/icons/project";
}

QString RamTemplateStep::details() const
{
    switch(type())
    {
    case PreProduction:
        return "Pre-production";
    case ShotProduction:
        return "Shot production";
    case AssetProduction:
        return "Asset production";
    case PostProduction:
        return "Post-production";
    }
    return "";
}

void RamTemplateStep::edit(bool show)
{
    if (!ui_editWidget) ui_editWidget = createEditFrame(new TemplateStepEditWidget(this));

    if (show) showEdit( ui_editWidget );
}

float RamTemplateStep::estimationHard() const
{
    return getData(KEY_EstimationHard).toDouble(2);
}

void RamTemplateStep::setEstimationHard(float newEstimationHard)
{
    insertData(KEY_EstimationHard, newEstimationHard);
}

float RamTemplateStep::estimationMedium() const
{
    return getData(KEY_EstimationMedium).toDouble(1);
}

void RamTemplateStep::setEstimationMedium(float newEstimationMedium)
{
    insertData(KEY_EstimationMedium, newEstimationMedium);
}

float RamTemplateStep::estimationEasy() const
{
    return getData(KEY_EstimationEasy).toDouble(0.5);
}

void RamTemplateStep::setEstimationEasy(float newEstimationEasy)
{
    insertData(KEY_EstimationEasy, newEstimationEasy);
}

float RamTemplateStep::estimationVeryEasy() const
{
    return getData(KEY_EstimationVeryEasy).toDouble(0.2);
}

void RamTemplateStep::setEstimationVeryEasy(float newEstimationVeryEasy)
{
    insertData(KEY_EstimationVeryEasy, newEstimationVeryEasy);
}

RamTemplateStep::EstimationMethod RamTemplateStep::estimationMethod() const
{
    QString methodStr = getData(KEY_EstimationMethod).toString(ENUMVALUE_Shot);
    if (methodStr == ENUMVALUE_Shot) return EstimatePerShot;
    else return EstimatePerSecond;
}

void RamTemplateStep::setEstimationMethod(const EstimationMethod &newEstimationMethod)
{
    switch(newEstimationMethod)
    {
    case EstimatePerShot:
        insertData(KEY_EstimationMethod, ENUMVALUE_Shot);
        break;
    case EstimatePerSecond:
        insertData(KEY_EstimationMethod, ENUMVALUE_Second);
        break;
    }
}

void RamTemplateStep::setEstimationMethod(const QString &newEstimationMethod)
{
    insertData(KEY_EstimationMethod, newEstimationMethod);
}

// PROTECTED //

void RamTemplateStep::construct()
{
    m_existingObjects[m_uuid] = this;
    m_icon = ":/icons/step";
    m_editRole = Admin;

    m_applications = createModel(RamObject::Application, "applications");
}
