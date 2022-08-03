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
}

RamTemplateStep *RamTemplateStep::getObject(QString uuid, bool constructNew)
{
    RamObject *obj = RamObject::getObject(uuid);
    if (!obj && constructNew) return new RamTemplateStep( uuid );
    return qobject_cast<RamTemplateStep*>( obj );
}

// PUBLIC //

RamTemplateStep::RamTemplateStep(QString shortName, QString name) :
    RamObject(shortName, name, TemplateStep)
{
    construct();

    QJsonObject d = data();

    m_applications = new RamObjectList<RamApplication*>(shortName + "-Apps", name + " | Applications", this);
    d.insert("applications", m_applications->uuid());

    setData(d);
}

RamObjectList<RamApplication *> *RamTemplateStep::applications() const
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

QColor RamTemplateStep::color() const
{
    return QColor( getData("color").toString("#434343") );
}

void RamTemplateStep::setColor(const QColor &newColor)
{
    insertData("color", newColor.name());
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

RamTemplateStep::RamTemplateStep(QString uuid):
    RamObject(uuid, TemplateStep)
{
    construct();

    m_applications = RamObjectList<RamApplication*>::getObject( getData("applications").toString(), true );
}

void RamTemplateStep::construct()
{
    m_icon = ":/icons/step";
    m_editRole = Admin;
}
