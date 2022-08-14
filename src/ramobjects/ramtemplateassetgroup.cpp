#include "ramtemplateassetgroup.h"

#include "templateassetgroupeditwidget.h"

// STATIC //

RamTemplateAssetGroup *RamTemplateAssetGroup::getObject(QString uuid, bool constructNew)
{
    RamObject *obj = RamObject::getObject(uuid);
    if (!obj && constructNew) return new RamTemplateAssetGroup( uuid );
    return qobject_cast<RamTemplateAssetGroup*>( obj );
}

RamTemplateAssetGroup *RamTemplateAssetGroup::c(RamObject *o)
{
    return qobject_cast<RamTemplateAssetGroup*>(o);
}

// PUBLIC //

RamTemplateAssetGroup::RamTemplateAssetGroup(QString shortName, QString name) :
    RamObject(shortName, name, AssetGroup)
{
    construct();
}

// PUBLIC SLOTS //

void RamTemplateAssetGroup::edit(bool show)
{
    if (!ui_editWidget) setEditWidget(new TemplateAssetGroupEditWidget(this));

    if (show) showEdit();
}

// PROTECTED //

RamTemplateAssetGroup::RamTemplateAssetGroup(QString uuid):
    RamObject(uuid, TemplateAssetGroup)
{
    construct();
}

// PRIVATE //

void RamTemplateAssetGroup::construct()
{
    m_icon = ":/icons/asset-group";
    m_editRole = Admin;
}
