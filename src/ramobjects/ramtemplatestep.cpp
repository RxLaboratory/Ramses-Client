#include "ramtemplatestep.h"

#include "templatestepeditwidget.h"

// STATIC //

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
    if (typeName == "All") return All;
    return All;
}

RamTemplateStep *RamTemplateStep::get(QString uuid)
{
    return c( RamObject::get(uuid, TemplateStep) );
}

RamTemplateStep *RamTemplateStep::c(RamObject *o)
{
    return qobject_cast<RamTemplateStep*>(o);
}

// PUBLIC //

RamTemplateStep::RamTemplateStep(QString shortName, QString name) :
    RamObject(shortName, name, TemplateStep)
{
    construct();

    QJsonObject d = data();

    m_applications = new RamObjectList(shortName + "-Apps", name + " | Applications", Application, RamObjectList::ListObject, this);
    d.insert("applications", m_applications->uuid());

    setData(d);
}

RamTemplateStep::RamTemplateStep(QString uuid):
    RamObject(uuid, TemplateStep)
{
    construct();

    m_applications = RamObjectList::get( getData("applications").toString(), ObjectList );
}

RamObjectList *RamTemplateStep::applications() const
{
    return m_applications;
}

RamTemplateStep::Type RamTemplateStep::type() const
{
    QString typeStr = getData("type").toString();

    if (typeStr == "pre") return PreProduction;
    else if (typeStr == "asset") return AssetProduction;
    else if (typeStr == "shot") return ShotProduction;
    else if (typeStr == "post") return PostProduction;
    else return All;
}

void RamTemplateStep::setType(const Type &type)
{
    switch(type){
    case PreProduction:
        insertData("type", "pre");
        break;
    case AssetProduction:
        insertData("type", "asset");
        break;
    case ShotProduction:
        insertData("type", "shot");
        break;
    case PostProduction:
        insertData("type", "post");
        break;
    case All:
        insertData("type", "all");
        break;
    }
}

void RamTemplateStep::setType(QString type)
{
    insertData("type", type);
}

QString RamTemplateStep::publishSettings() const
{
    return getData("publishSettings").toString();
}

void RamTemplateStep::setPublishSettings(const QString &newPublishSettings)
{
    insertData("publishSettings", newPublishSettings);
}

float RamTemplateStep::estimationVeryHard() const
{
    return getData("estimationVeryHard").toDouble();
}

void RamTemplateStep::setEstimationVeryHard(float newEstimationVeryHard)
{
    insertData("estimationVeryHard", newEstimationVeryHard);
}

QString RamTemplateStep::iconName() const
{
    switch(type())
    {
    case PreProduction:
        return ":/icons/asset";
    case ShotProduction:
        return ":/icons/shot";
    case AssetProduction:
        return ":/icons/asset";
    case PostProduction:
        return ":/icons/film";
    case All:
        return ":/icons/step";
    }
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
    case All:
        return "";
    }
}

void RamTemplateStep::edit(bool show)
{
    if (!ui_editWidget) setEditWidget(new TemplateStepEditWidget(this));

    if (show) showEdit();
}

float RamTemplateStep::estimationHard() const
{
    return getData("estimationHard").toDouble();
}

void RamTemplateStep::setEstimationHard(float newEstimationHard)
{
    insertData("estimationHard", newEstimationHard);
}

float RamTemplateStep::estimationMedium() const
{
    return getData("estimationMedium").toDouble();
}

void RamTemplateStep::setEstimationMedium(float newEstimationMedium)
{
    insertData("estimationMedium", newEstimationMedium);
}

float RamTemplateStep::estimationEasy() const
{
    return getData("estimationEasy").toDouble();
}

void RamTemplateStep::setEstimationEasy(float newEstimationEasy)
{
    insertData("estimationEasy", newEstimationEasy);
}

float RamTemplateStep::estimationVeryEasy() const
{
    return getData("estimationVeryEasy").toDouble();
}

void RamTemplateStep::setEstimationVeryEasy(float newEstimationVeryEasy)
{
    insertData("estimationVeryEasy", newEstimationVeryEasy);
}

RamTemplateStep::EstimationMethod RamTemplateStep::estimationMethod() const
{
    QString methodStr = getData("estimationMethod").toString("shot");
    if (methodStr == "shot") return EstimatePerShot;
    else return EstimatePerSecond;
}

void RamTemplateStep::setEstimationMethod(const EstimationMethod &newEstimationMethod)
{
    switch(newEstimationMethod)
    {
    case EstimatePerShot:
        insertData("estimationMethod", "shot");
        break;
    case EstimatePerSecond:
        insertData("estimationMethod", "second");
        break;
    }
}

// PROTECTED //

void RamTemplateStep::construct()
{
    m_icon = ":/icons/step";
    m_editRole = Admin;
}
