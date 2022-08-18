#include "ramshot.h"

#include "ramasset.h"
#include "ramassetgroup.h"
#include "ramproject.h"
#include "ramsequence.h"
#include "shoteditwidget.h"

// STATIC //

RamShot *RamShot::get(QString uuid)
{
    return c( RamObject::get(uuid, Shot) );
}

RamShot *RamShot::c(RamObject *obj)
{
    return qobject_cast<RamShot*>(obj);
}

// PUBLIC //

RamShot::RamShot(QString shortName, QString name, RamSequence *sequence):
    RamItem(shortName, name, Shot, sequence->project())
{
    Q_ASSERT_X(sequence, "RamAsset(shortname, name, assetgroup)", "Sequence can't be null!");
    construct();
    insertData("sequence", sequence->uuid() );
}

RamShot::RamShot(QString uuid):
    RamItem(uuid, Shot)
{
    construct();
}

RamSequence *RamShot::sequence() const
{
    return RamSequence::get( getData("sequence").toString() );
}

void RamShot::setSequence(RamObject *sequence)
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

RamObjectList *RamShot::assets() const
{
    return m_assets;
}

RamAsset *RamShot::assetAt(int row) const
{
    return qobject_cast<RamAsset*>( m_assets->at(row) );
}

QString RamShot::filterUuid() const
{
    return getData("sequence").toString();
}

QString RamShot::details() const
{
    RamProject *proj = project();
    if (!proj) return tr("Invalid Shot.\n\nMaybe the database needs to be repaired.");

    QString details = tr("Duration:") + " " +
                    QString::number(duration(), 'f', 2) +
                    " s | " +
                    QString::number(duration() * proj->framerate(), 'f', 2) +
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

QString RamShot::folderPath() const
{
    RamProject *proj = project();
    if (!proj) return "";
    return proj->path(RamObject::ShotsFolder) + "/" + proj->shortName() + "_S_" + shortName();
}

// PRIVATE //

void RamShot::construct()
{
    m_icon = ":/icons/shot";
    m_editRole = ProjectAdmin;
    getCreateLists();
}

void RamShot::getCreateLists()
{
    QJsonObject d = data();

    QString uuid = d.value("assets").toString();
    if (uuid == "") m_assets = new RamObjectList("assets", "Assets", Asset, RamObjectList::ListObject, this);
    else m_assets = RamObjectList::get( uuid, ObjectList);
    m_assets->setParent(this);
    d.insert("assets", m_assets->uuid());

    setData(d);
}




























