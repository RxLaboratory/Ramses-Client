#include "ramabstractitem.h"

#include "ramproject.h"

// PUBLIC //

RamAbstractItem::RamAbstractItem(QString shortName, QString name, ObjectType type, RamProject *project) :
    RamObject(shortName, name, type, project)
{
    construct();

    QJsonObject d = data();
    d.insert("project", project->uuid());
    setData(d);
}

RamAbstractItem::RamAbstractItem(QString uuid, ObjectType type):
    RamObject(uuid, type)
{
    construct();

    QJsonObject d = data();

    // Get project first
    QString projectUuid = d.value("project").toString("");
    if (projectUuid != "")
    {
        RamProject *project = RamProject::get(projectUuid);
        setParent(project);
    }
}

RamProject *RamAbstractItem::project() const
{
    return RamProject::get( getData("project").toString("none") );
}

// PRIVATE //

void RamAbstractItem::construct()
{
    m_icon = ":/icons/asset";
    m_editRole = Admin;
}
