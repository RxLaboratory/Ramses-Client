#include "ramshot.h"
#include "ramsequence.h"
#include "ramproject.h"
#include "shoteditwidget.h"

RamShot::RamShot(QString shortName, RamSequence *sequence, QString name, QString uuid):
    RamItem(shortName, sequence->project(), name, uuid)
{
    m_icon = ":/icons/shot";
    m_editRole = ProjectAdmin;

    m_assets = new RamObjectList("SHOTASSETS", "Assets", this);
    setObjectType(Shot);
    setProductionType(RamStep::ShotProduction);
    m_sequence = sequence;
    m_sequenceConnection = connect(sequence, SIGNAL(removed(RamObject*)),this,SLOT(remove()));
    m_filterUuid = sequence->uuid();
    m_dbi->createShot(m_shortName, m_name, m_sequence->uuid(), m_uuid);

    this->setObjectName( "RamShot " + m_shortName );

    connect(m_assets, SIGNAL(rowsInserted(QModelIndex,int,int)),this,SLOT(assetAssigned(QModelIndex,int,int)));
    connect(m_assets, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)),this,SLOT(assetUnassigned(QModelIndex,int,int)));
}

RamShot::~RamShot()
{

}

RamSequence *RamShot::sequence() const
{
    return m_sequence;
}

void RamShot::setSequence(RamSequence *sequence)
{
    if (m_sequence->is(sequence)) return;
    m_dirty = true;

    disconnect(m_sequenceConnection);

    setParent(sequence);
    m_sequence = sequence;
    m_filterUuid = sequence->uuid();

    m_sequenceConnection = connect(sequence, SIGNAL(removed(RamObject*)),this,SLOT(remove()));

    emit dataChanged(this);
}

qreal RamShot::duration() const
{
    if (m_duration == 0) return 5;
    return m_duration;
}

void RamShot::setDuration(const qreal &duration)
{
    if (m_duration == duration) return;
    m_dirty = true;
    m_duration = duration;
    emit dataChanged(this);
}

RamShot *RamShot::shot(QString uuid)
{
    return qobject_cast<RamShot*>( RamObject::obj(uuid) );
}

void RamShot::update()
{
    if (m_orderChanged)
    {
        m_dbi->setShotOrder(m_uuid, m_order);
        m_orderChanged = false;
    }

    if(!m_dirty) return;
    RamObject::update();
    m_dbi->updateShot(m_uuid, m_shortName, m_name, m_sequence->uuid(), m_duration, m_comment);
}

bool RamShot::move(int index)
{
    if (!RamObject::move(index)) return false;

    m_dbi->moveShot(m_uuid, m_order);
    return true;
}

void RamShot::edit(bool show)
{
    if (!m_editReady)
    {
        ShotEditWidget *w = new ShotEditWidget(this);
        setEditWidget(w);
        m_editReady = true;
    }
    if (show) showEdit();
}

QString RamShot::folderPath() const
{
    RamProject *p = m_sequence->project();
    return p->path(RamObject::ShotsFolder) + "/" + p->shortName() + "_S_" + m_shortName;
}

void RamShot::assetAssigned(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent)

    for (int i  = first; i <= last; i++)
    {
        RamObject *assetObj = m_assets->at(i);
        m_dbi->assignAsset(m_uuid, assetObj->uuid());
    }
}

void RamShot::assetUnassigned(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent)

    for (int i  = first; i <= last; i++)
    {
        RamObject *assetObj = m_assets->at(i);
        m_dbi->unassignAsset(m_uuid, assetObj->uuid());
    }
}

void RamShot::removeFromDB()
{
    m_dbi->removeShot(m_uuid);
}

RamObjectList *RamShot::assets() const
{
    return m_assets;
}
