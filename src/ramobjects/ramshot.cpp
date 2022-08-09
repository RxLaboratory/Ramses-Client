#include "ramshot.h"

#include "ramasset.h"
#include "ramassetgroup.h"
#include "ramproject.h"
#include "ramsequence.h"
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
    m_assets = new RamItemTable(shortName + "-Assets", name + " Assets", sequence->project()->steps(), this);
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

RamItemTable *RamShot::assets() const
{
    return m_assets;
}

QString RamShot::filterUuid() const
{
    return getData("sequence").toString();
}

QString RamShot::details() const
{
    QString details = "Duration: " +
                    QString::number(duration(), 'f', 2) +
                    " s | " +
                    QString::number(duration() * m_project->framerate(), 'f', 2) +
                    " f";

    // List assigned assets
    QMap<QString,QStringList> assts;
    for (int i = 0; i < assets()->rowCount(); i++)
    {
        RamAsset *asset = qobject_cast<RamAsset*>(assets()->at(i));
        QString agName = asset->assetGroup()->name();
        QStringList ag = assts.value( agName );
        ag << asset->shortName();
        assts[ agName ] = ag;
    }
    QMapIterator<QString,QStringList> i(assts);
    while(i.hasNext())
    {
        i.next();
        details = details + "\n" % i.key() + " ► " + i.value().join(", ");
    }

    return details;
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
    m_assets = RamItemTable::getObject( d.value("assets").toString(), true );
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




























