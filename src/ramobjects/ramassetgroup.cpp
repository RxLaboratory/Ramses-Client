#include "ramassetgroup.h"
#include "ramproject.h"
#include "ramses.h"

RamAssetGroup::RamAssetGroup(QString shortName, QString name, QString uuid) :
    RamObject(shortName, name, uuid, Ramses::instance())
{
    this->setObjectType(AssetGroup);
    m_project = nullptr;
    _template = true;
    if (_template) m_dbi->createTemplateAssetGroup(m_shortName, m_name, m_uuid);

    this->setObjectName( "RamAssetGroup (template) " + m_shortName );
}

RamAssetGroup::RamAssetGroup(QString shortName, RamProject *project, QString name, QString uuid):
    RamObject(shortName, name, uuid, project)
{
    this->setObjectType(AssetGroup);
    m_project = project;
    _template = false;
    m_dbi->createAssetGroup(m_shortName, m_name, m_project->uuid(), m_uuid);

    this->setObjectName( "RamAssetGroup " + m_shortName);
}

RamAssetGroup::~RamAssetGroup()
{
    if (_template) m_dbi->removeTemplateAssetGroup(m_uuid);
    else m_dbi->removeAssetGroup(m_uuid);
}

bool RamAssetGroup::isTemplate() const
{
    return _template;
}

RamAssetGroup *RamAssetGroup::createFromTemplate(QString projectUuid)
{
    // Create
    RamAssetGroup *assetGroup = new RamAssetGroup(m_shortName, m_name, projectUuid);
    return assetGroup;
}

RamProject *RamAssetGroup::project() const
{
    return m_project;
}

RamAssetGroup *RamAssetGroup::assetGroup(QString uuid)
{
    return qobject_cast<RamAssetGroup*>( RamObject::obj(uuid));
}

void RamAssetGroup::update()
{
    if (!m_dirty) return;
    RamObject::update();
    if (_template) m_dbi->updateTemplateAssetGroup(m_uuid, m_shortName, m_name);
    else m_dbi->updateAssetGroup(m_uuid, m_shortName, m_name);
}

