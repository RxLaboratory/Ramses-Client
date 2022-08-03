#include "ramshot.h"

#include "ramproject.h"
#include "shoteditwidget.h"

// STATIC //

RamShot *RamShot::getObject(QString uuid, bool constructNew )
{
    RamObject *obj = RamItem::getObject(uuid);
    if (!obj && constructNew) return new RamShot( uuid );
    return qobject_cast<RamShot*>( obj );
}

// PUBLIC //

RamShot::RamShot(QString shortName, QString name, RamSequence *sequence):
    RamItem(shortName, name, RamStep::ShotProduction, sequence->project())
{
    Q_ASSERT_X(sequence, "RamAsset(shortname, name, assetgroup)", "Sequence can't be null!");
    construct();
    insertData("sequence", sequence->uuid() );
    m_assets = new RamObjectList<RamAsset *>(shortName + "-Assets", name + " Assets", this);
}

RamSequence *RamShot::sequence() const
{
    return RamSequence::getObject( getData("sequence").toString(), true);
}

void RamShot::setSequence(RamSequence *sequence)
{
    insertData("sequence", sequence->uuid() );
}

qreal RamShot::duration() const
{
    return getData("duration").toDouble(5);
}

void RamShot::setDuration(const qreal &duration)
{
    insertData("duration", duration);
}

RamObjectList<RamAsset *> *RamShot::assets() const
{
    return m_assets;
}

QString RamShot::filterUuid() const
{
    return getData("sequence").toString();
}

// PUBLIC SLOTS //

void RamShot::edit(bool show)
{
    if (!ui_editWidget) setEditWidget(new ShotEditWidget(this));

    if (show) showEdit();
}

// PROTECTED //

RamShot::RamShot(QString uuid):
    RamItem(uuid)
{
    construct();
    // Get asset list
    QJsonObject d = data();
    m_assets = RamObjectList<RamAsset *>::getObject( d.value("assets").toString(), true );
}

QString RamShot::folderPath() const
{
    return m_project->path(RamObject::ShotsFolder) + "/" + m_project->shortName() + "_S_" + shortName();
}

// PRIVATE //

void RamShot::construct()
{
    m_icon = ":/icons/shot";
    m_editRole = ProjectAdmin;
    m_productionType = RamStep::ShotProduction;
}




























