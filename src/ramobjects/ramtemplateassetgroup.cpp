#include "ramtemplateassetgroup.h"

#include "templateassetgroupeditwidget.h"

// STATIC //

RamTemplateAssetGroup *RamTemplateAssetGroup::get(QString uuid)
{
    return c( RamObject::get(uuid, TemplateAssetGroup) );
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

RamTemplateAssetGroup::RamTemplateAssetGroup(QString uuid):
    RamObject(uuid, TemplateAssetGroup)
{
    construct();
}

// PUBLIC SLOTS //

void RamTemplateAssetGroup::edit(bool show)
{
    if (!ui_editWidget) setEditWidget(new TemplateAssetGroupEditWidget(this));

    if (show) showEdit();
}

// PRIVATE //

void RamTemplateAssetGroup::construct()
{
    m_icon = ":/icons/asset-group";
    m_editRole = Admin;
}
