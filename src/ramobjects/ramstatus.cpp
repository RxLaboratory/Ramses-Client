﻿#include "ramstatus.h"

#include "ramitem.h"
#include "ramworkingfolder.h"
#include "statuseditwidget.h"
#include "ramses.h"
#include "ramfilemetadatamanager.h"

// PROTECTED //

RamStatus *RamStatus::getObject(QString uuid, bool constructNew)
{
    RamObject *obj = RamObject::getObject(uuid);
    if (!obj && constructNew) return new RamStatus( uuid );
    return qobject_cast<RamStatus*>( obj );
}

RamStatus *RamStatus::copy(RamStatus *other, RamUser *user)
{
    RamStatus *status = new RamStatus(
                user,
                other->item(),
                other->step()
                );

    status->setState( other->state() );
    status->setCompletionRatio( other->completionRatio() );
    status->setVersion( other->version() );
    status->setTimeSpent( other->timeSpent() );
    status->assignUser( other->assignedUser() );
    status->setDifficulty( other->difficulty() );
    status->setGoal( other->goal() );
    status->item()->addStatus(status);
    status->setUseAutoEstimation( other->useAutoEstimation() );
    status->setComment( other->comment() );

    return status;
}

RamStatus *RamStatus::noStatus(RamItem *item, RamStep *step)
{
    RamStatus *no = new RamStatus(
                Ramses::instance()->ramsesUser(),
                item,
                step,
                true);
    return no;
}

float RamStatus::hoursToDays(int hours)
{
    // 1 day ( more or less )
    if (hours <= 0) return 0;
    if (hours == 1) return 0.15;
    if (hours == 2) return 0.25;
    if (hours == 3) return 0.4;
    if (hours == 4) return 0.5;
    if (hours == 5) return 0.75;
    if (hours <= 6) return 0.85;
    if (hours <= 9) return 1;

    // Approximately 2 days
    if (hours < 11) return 1.25;
    if (hours < 14) return 1.5;
    if (hours < 18) return 2;

    // More than that, 8h/day
    int intDays = hours / 8;
    return intDays;
}

int RamStatus::daysToHours(float days)
{
    // let's use an 8h-day
    return days*8;
}

// PUBLIC //

RamStatus::RamStatus(RamUser *user, RamItem *item, RamStep *step, bool isVirtual):
    RamObject(item->shortName() + "-" + step->shortName(),
              item->name() + " | " + step->name(),
              Status,
              item,
              isVirtual)
{
    construct();

    m_user = user;
    m_step = step;
    m_item = item;

    QJsonObject d = data();

    d.insert("user", user->uuid());
    d.insert("item", item->uuid());
    if (item->productionType() == RamStep::ShotProduction) d.insert("itemType", "shot");
    else if (item->productionType() == RamStep::AssetProduction) d.insert("itemType", "asset");
    else d.insert("itemType", "item");

    d.insert("step", step->uuid());

    d.insert("state", Ramses::instance()->noState()->uuid());
    d.insert("completionRatio", 0);

    setData(d);

    connect(state(), SIGNAL(removed(RamObject*)), this, SLOT(stateRemoved()));
}

RamUser *RamStatus::user() const
{
    return m_user;
}

RamStep *RamStatus::step() const
{
    return m_step;
}

RamItem *RamStatus::item() const
{
    return m_item;
}

bool RamStatus::isNoState() const
{
    RamState *noState = Ramses::instance()->noState();
    return noState->is(state());
}

int RamStatus::completionRatio() const
{
    return getData("completionRatio").toInt(50);
}

void RamStatus::setCompletionRatio(int completionRatio)
{
    insertData("completionRatio", completionRatio);
}

RamState *RamStatus::state() const
{
    return RamState::getObject( getData("state").toString(), true);
}

void RamStatus::setState(RamState *newState)
{
    disconnect(state(), nullptr, this, nullptr);

    QJsonObject d = data();

    d.insert("state", newState->uuid());
    d.insert( "completionRatio", newState->completionRatio() );

    connect(newState, SIGNAL(removed(RamObject*)), this, SLOT(stateRemoved()));

    setData(d);
    m_step->computeEstimation();
}

int RamStatus::version() const
{
    return getData("version").toInt();
}

void RamStatus::setVersion(int version)
{
    insertData("version", version);
}

QDateTime RamStatus::date() const
{
    return QDateTime::fromString( getData("date").toString(), "yyyy-MM-dd hh:mm:ss");
}

void RamStatus::setDate(const QDateTime &date)
{
    insertData("date", date.toString("yyyy-MM-dd hh:mm:ss"));
}

bool RamStatus::isPublished() const
{
    return getData("published").toBool();
}

void RamStatus::setPublished(bool published)
{
    insertData("published", published);
}

RamUser *RamStatus::assignedUser() const
{
    return RamUser::getObject( getData("assignedUser").toString("none"), true );
}

void RamStatus::assignUser(RamUser *assignedUser)
{
    disconnect(assignedUser, nullptr, this, nullptr);
    if (!assignedUser) insertData("assignedUser", "none");
    else {
        insertData("assignedUser", assignedUser->uuid());
        connect(assignedUser, SIGNAL(removed(RamObject*)), this, SLOT(assignedUserRemoved()));
    }
}

qint64 RamStatus::timeSpent() const
{
    qint64 ts = getData("timeSpent").toInt();
    if (ts == 0)
    {
        // Detect
        RamFileMetaDataManager mdm( this->path(RamObject::VersionsFolder) );
        if (!mdm.isValid()) return ts;
        return mdm.getTimeRange();
    }
    return ts;
}

void RamStatus::setTimeSpent(const float &ts)
{
    QJsonObject d = data();
    d.insert("timeSpent", ts);
    d.insert("manualTimeSpent", true);
    setData(d);
    m_step->computeEstimation();
}

bool RamStatus::isTimeSpentManual() const
{
    return getData("manualTimeSpent").toBool(false);
}

RamStatus::Difficulty RamStatus::difficulty() const
{
    QString dffclt = getData("difficulty").toString("medium");
    if (dffclt == "veryEasy") return VeryEasy;
    else if (dffclt == "easy") return Easy;
    else if (dffclt == "medium") return Medium;
    else if (dffclt == "hard") return Hard;
    else if (dffclt == "veryHard") return VeryHard;
}

void RamStatus::setDifficulty(Difficulty newDifficulty)
{
    switch(newDifficulty) {
    case VeryEasy:
        insertData("difficulty", "veryEasy");
        break;
    case Easy:
        insertData("difficulty", "easy");
        break;
    case Medium:
        insertData("difficulty", "medium");
        break;
    case Hard:
        insertData("difficulty", "hard");
        break;
    case VeryHard:
        insertData("difficulty", "veryHard");
        break;
    }

    m_step->computeEstimation();
}

float RamStatus::goal() const
{
    // If state is none, 0!
    RamState *noState = Ramses::instance()->noState();
    if (noState->is(state())) return 0.0;

    float g = getData("goal").toDouble();

    return g;
}

void RamStatus::setGoal(float newGoal)
{
    insertData("goal", newGoal);
    m_step->computeEstimation();
}

float RamStatus::estimation() const
{
    return estimation( difficulty() );
}

float RamStatus::estimation(int difficulty) const
{
    float est = 0.0;

    // If state is none, 0!
    RamState *noState = Ramses::instance()->noState();
    if (noState->is(state())) return 0.0;

    switch (difficulty)
    {
    case VeryEasy:
    {
        est = m_step->estimationVeryEasy();
        break;
    }
    case Easy:
    {
        est = m_step->estimationEasy();
        break;
    }
    case Medium:
    {
        est = m_step->estimationMedium();
        break;
    }
    case Hard:
    {
        est = m_step->estimationHard();
        break;
    }
    case VeryHard:
    {
        est = m_step->estimationVeryHard();
        break;
    }
    default:
    {
        est = m_step->estimationMedium();
    }
    }

    // Multiply by duration and num assets if shot
    if ( m_item->objectType() == RamObject::Shot)
    {
        RamShot *shot = qobject_cast<RamShot*>( m_item );
        if (m_step->estimationMethod() == RamStep::EstimatePerSecond)
            est *= shot->duration();
        RamAssetGroup *ag = m_step->estimationMultiplyGroup();
        if (ag)
        {
            // count assets
            int numAssets = 0;
            for (int i = 0; i < shot->assets()->count(); i++)
            {
                RamAsset *asset = qobject_cast<RamAsset*>( shot->assets()->at(i) );
                if (asset->assetGroup()->is(ag)) numAssets++;
            }
            if (numAssets > 0) est *= numAssets;
        }
    }
    return est;
}

bool RamStatus::useAutoEstimation() const
{
    RamState *noState = Ramses::instance()->noState();
    if (noState->is(state())) return true;

    return getData("useAutoEstimation").toBool(true);
}

void RamStatus::setUseAutoEstimation(bool newAutoEstimation)
{
    QJsonObject d = data();

    d.insert("useAutoEstimation", newAutoEstimation);
    if (!newAutoEstimation && d.value("goal").toDouble() <= 0) d.insert("goal", estimation());

    setData(d);

    m_step->computeEstimation();
}

float RamStatus::latenessRatio() const
{
    QJsonObject d = data();

    float completionRatio = d.value("completionRatio").toInt(50) / 100.0;

    float est;
    if (useAutoEstimation()) est = estimation();
    else est = d.value("goal").toDouble();

    if (est <= 0) return 1;
    if (completionRatio <= 0) return 1;

    float timeRatio = hoursToDays(d.value("timeSpent").toDouble()/3600) / est;

    return timeRatio / completionRatio;
}

RamWorkingFolder RamStatus::workingFolder() const
{
    return RamWorkingFolder(this->path());
}

QString RamStatus::createFileFromTemplate(QString filePath) const
{
    if (!QFileInfo::exists( filePath)) return "";

    // Generate destination name
    RamNameManager nm;
    nm.setFileName(filePath);
    nm.setProject( m_item->project()->shortName() );
    nm.setStep( m_step->shortName() );
    nm.setResource( nm.shortName() );
    if (m_item->objectType() == Asset) nm.setType("A");
    else if (m_item->objectType() == Shot) nm.setType("S");
    else nm.setType("G");
    nm.setShortName( m_item->shortName() );
    if (nm.resource() == "Template") nm.setResource("");

    QString destination = QDir(
                path(NoFolder, true)
                ).filePath( nm.fileName() );

     if (QFileInfo::exists( destination ))
     {
         nm.setResource( nm.resource() + "+new+");
         destination = QDir(
                     path(NoFolder, true)
                     ).filePath( nm.fileName() );
     }

     if (QFile::copy(filePath, destination)) return destination;
     return "";
}

QString RamStatus::createFileFromResource(QString filePath) const
{
    // Get infos
    RamNameManager nm;
    nm.setFileName(filePath);

    RamWorkingFolder wf( QFileInfo(filePath).path() );

    // Get the latest publish if any
    QStringList templateFiles = wf.publishedFiles(nm.resource());
    foreach (QString tFile, templateFiles)
    {
        // Check the file extension, which should be the same
        if ( QFileInfo(tFile).suffix() == nm.extension()) return createFileFromTemplate(tFile);
    }
    // Try with the first published file
    if (templateFiles.count() > 0) return createFileFromTemplate(templateFiles.at(0));

    // Use the given file path
    return createFileFromTemplate( filePath );
}

QString RamStatus::restoreVersionFile(QString fileName) const
{
    QString restoredPath;
    RamNameManager nm;
    nm.setFileName( fileName );
    QString resource = nm.resource();
    QString v = QString::number( nm.version() );
    while(v.count() < 3)
        v = "0" + v;
    resource += "+restored-v" + v + "+";
    nm.setResource(resource);
    nm.setVersion(-1);

    QString versionPath = QDir( path(RamObject::VersionsFolder) ).filePath( fileName );
    restoredPath = QDir( path()).filePath( nm.fileName() );

    // Copy
    QFile::copy(versionPath, restoredPath);

    return restoredPath;

}

QString RamStatus::previewImagePath() const
{
    QDir previewDir = path(RamObject::PreviewFolder);
    QStringList filters;
    filters << "*.jpg" << "*.png" << "*.jpeg" << "*.gif";
    QStringList images = previewDir.entryList(filters, QDir::Files );

    if (images.count() == 0) return "";

    RamNameManager nm;

    RamProject *p = m_item->project();

    foreach(QString file, images)
    {
        if (nm.setFileName(file))
        {
            if (nm.project().toLower() != p->shortName().toLower()) continue;
            if (nm.step().toLower() != m_step->shortName().toLower()) continue;
            if (nm.shortName().toLower() != m_item->shortName().toLower()) continue;
            return previewDir.filePath( file );
        }
    }

    // Not found, return the first one
    return previewDir.filePath( images.at(0) );
}

QString RamStatus::details() const
{
    QString details;

    RamUser *au = assignedUser();
    if (au) details = "Assigned to: " +
            au->name() %
            "\nDifficulty: ";
    else details = "Not assigned\nDifficulty: ";

    switch( difficulty() )
    {
    case RamStatus::VeryEasy: { details += "Very easy"; break; }
    case RamStatus::Easy: { details += "Easy"; break; }
    case RamStatus::Medium: { details += "Medium"; break; }
    case RamStatus::Hard: { details += "Hard"; break; }
    case RamStatus::VeryHard: { details += "Very hard"; break; }
    }

    qint64 timeSpentSecs = timeSpent();
    // Convert to hours
    int timeSpentHours = timeSpentSecs / 3600;

    // Ratio
    float est = 0;
    bool useAuto = useAutoEstimation();
    if (useAuto) est = estimation();
    else est = goal();
    float timeSpentDays = RamStatus::hoursToDays( timeSpentHours/3600 );

    // Estimation or goal
    if (timeSpentHours > 0)
    {
        details = details +
            "\nTime spent: " +
            QString::number(timeSpentHours) +
            " hours (" +
            QString::number(timeSpentDays, 'f', 0) +
            " days) / " +
            QString::number(est, 'f', 1) +
            " days ";
        if (useAutoEstimation()) details += "(estimated)";
        else details += "(goal)";
    }
    else if (useAuto) details +=
            "\nEstimation: " +
            QString::number(est, 'f', 1) +
            " days";
    else details +=
            "\nGoal: " +
            QString::number(est, 'f', 1) +
            " days";

   if (isPublished()) details += "\n► Published";

   return details;

}

QString RamStatus::subDetails() const
{
    //subdetails
    QString dateFormat = "yyyy-MM-dd hh:mm:ss";
    RamUser *u = Ramses::instance()->currentUser();
    if (u)
    {
        QSettings *uSettings = u->settings();
        dateFormat = uSettings->value("ramses/dateFormat", dateFormat).toString();
    }
    return "Modified on: " +
            date().toString(dateFormat) +
            "\nBy: " +
            user()->name();
}

// PUBLIC SLOTS //

void RamStatus::edit(bool show)
{
    if (!ui_editWidget) setEditWidget(new StatusEditWidget(this));

    if (show) showEdit();
}

// PROTECTED //

RamStatus::RamStatus(QString uuid):
    RamObject(uuid, Status)
{
    construct();

    QJsonObject d = data();
    m_user = RamUser::getObject( d.value("user").toString(), true );

    QString itemType = d.value("itemType").toString("asset");
    if (itemType == "asset") {
        m_item = RamAsset::getObject( d.value("item").toString(), true);
    }
    else if (itemType == "shot") {
        m_item = RamShot::getObject( d.value("item").toString(), true);
    }
    else {
        m_item = RamItem::getObject( d.value("item").toString(), true);
    }

    m_step = RamStep::getObject( d.value("step").toString(), true);

    connectEvents();
}

QString RamStatus::folderPath() const
{
    RamProject *project = m_item->project();
    QString type = "_G_";
    if (m_item->objectType() == RamObject::Shot) type = "_S_";
    else if (m_item->objectType() == RamObject::Asset) type = "_A_";
    return m_item->path() + "/" + project->shortName() + type + m_item->shortName() + "_" + m_step->shortName();
}

// PRIVATE SLOTS //

void RamStatus::stateRemoved()
{
    this->setState( Ramses::instance()->wipState() );
}

void RamStatus::userRemoved()
{
    m_user = Ramses::instance()->removedUser();
    insertData("user", m_user->uuid());
}

void RamStatus::assignedUserRemoved()
{
    assignUser(nullptr);
}

// PRIVATE //

void RamStatus::construct()
{
    m_icon = ":/icons/status";
    m_editRole = Standard;
}

void RamStatus::connectEvents()
{
    connect(m_user, SIGNAL( removed(RamObject*)), this, SLOT(userRemoved()));
    connect(m_item, SIGNAL( removed(RamObject*)), this, SLOT(remove()));
    connect(m_step, SIGNAL( removed(RamObject*)), this, SLOT(remove()));
}
