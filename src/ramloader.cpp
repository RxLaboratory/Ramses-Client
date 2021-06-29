#include "ramloader.h"

RamLoader *RamLoader::_instance = nullptr;

RamLoader *RamLoader::instance()
{
    if (!_instance) _instance = new RamLoader();
    return _instance;
}

RamLoader::RamLoader(QObject *parent):
    QThread(parent)
{
    m_pm = ProcessManager::instance();
    m_ram = Ramses::instance();

    connect(this, &RamLoader::refresh, m_ram, &Ramses::refresh);
    connect(this, &RamLoader::ready, m_ram, &Ramses::init);
    connect(this, &RamLoader::projectReady, m_ram, &Ramses::projectReady);
}

void RamLoader::run()
{
    m_pm->start();
    m_pm->setTitle("Loading new data.");

    QString query = m_data.value("query").toString();

    if (query == "login") login( m_data.value("content").toObject() );
    else if (query == "getUsers") gotUsers( m_data.value("content").toArray());
    else if (query == "getProjects") gotProjects( m_data.value("content").toArray(), false);
    else if (query == "getProject") {
        qDebug() << "--- Loading Project Data ---";
        m_pm->setTitle("Loading project...");
        QString uuid = gotProject( m_data.value("content").toObject(), false);
        m_pm->setTitle("Selecting current project...");
        emit projectReady(uuid);
    }
    else if (query == "getTemplateSteps") gotTemplateSteps( m_data.value("content").toArray());
    else if (query == "getTemplateAssetGroups") gotTemplateAssetGroups( m_data.value("content").toArray());
    else if (query == "getStates") gotStates( m_data.value("content").toArray());
    else if (query == "getFileTypes") gotFileTypes( m_data.value("content").toArray());
    else if (query == "getApplications") gotApplications( m_data.value("content").toArray());
    else if (query == "init")
    {
        qDebug() << "--- Loading Initial Data ---";
        m_pm->setTitle("Getting Ramses data.");
        m_pm->setMaximum(7);
        QJsonObject content = m_data.value("content").toObject();
        gotUsers( content.value("users").toArray());
        gotTemplateSteps( content.value("templateSteps").toArray());
        gotTemplateAssetGroups( content.value("templateAssetGroups").toArray());
        gotStates( content.value("states").toArray());
        gotFileTypes( content.value("fileTypes").toArray());
        gotApplications( content.value("applications").toArray());
        gotProjects( content.value("projects").toArray(), true);
        emit ready();
    }

    m_pm->finish();
}

void RamLoader::newData(QJsonObject data)
{
    if (!data.value("success").toBool()) return;

    m_data = data;
    //start(QThread::LowPriority); //TODO FIX QObject: Cannot create children for a parent that is in a different thread.
    run();
}

void RamLoader::login(QJsonObject user)
{
    // Set the current user shortName
    m_currentUserShortName = user.value("shortName").toString("Guest");
    // Now we can ask Ramses to refresh all
    emit refresh();
}

void RamLoader::gotUsers(QJsonArray users)
{
    DBISuspender s;

    m_pm->increment();
    m_pm->setText("Loading projects...");
    qDebug() << "Loading projects";

    QStringList uuids;
    // Update projects
    for (int j = 0; j < users.count(); j++)
        uuids << gotUser( users.at(j).toObject() );

    RamObjectList *us = m_ram->users();

    // Remove deleted projects
    for (int i = us->count() - 1; i >= 0; i--)
    {
        RamObject *u = us->at(i);
        if (!uuids.contains(u->uuid()))
            u->remove();
    }

    us->sort();

    // Set the current and ramses user
    m_pm->setText("Selecting current user");
    qDebug() << "Selecting current user";

    for (int i = 0; i < us->count(); i++)
    {
        if (us->at(i)->shortName() == m_currentUserShortName)
        {
            m_ram->setCurrentUser( qobject_cast<RamUser*>( us->at(i) ));
            return;
        }
    }

    // Not found
    m_ram->setCurrentUser(nullptr);
}

QString RamLoader::gotUser(QJsonObject newU)
{
    DBISuspender s;
    QString uuid = newU.value("uuid").toString();

    RamUser *user = RamUser::user( uuid );
    if(!user) user = new RamUser(
                newU.value("shortName").toString(),
                newU.value("name").toString(),
                uuid
                );

    user->setFolderPath( newU.value("folderPath").toString());
    user->setRole( newU.value("role").toString("standard") );
    user->setComment( newU.value("comment").toString());

    m_ram->users()->append(user);

    return uuid;
}

void RamLoader::gotProjects(QJsonArray projects, bool init)
{
    DBISuspender s;

    m_pm->increment();
    m_pm->setText("Loading projects...");
    qDebug() << "Loading projects";

    QStringList uuids;
    // Update projects
    for (int j = 0; j < projects.count(); j++)
        uuids << gotProject( projects.at(j).toObject(), init );

    RamObjectList *_projects = m_ram->projects();

    // Remove deleted projects
    for (int i = _projects->count() - 1; i >= 0; i--)
    {
        RamObject *existingProject = _projects->at(i);
        if (!uuids.contains(existingProject->uuid()))
            existingProject->remove();
    }
}

QString RamLoader::gotProject(QJsonObject newP, bool init)
{
    DBISuspender s;
    QString uuid = newP.value("uuid").toString();

    RamProject *project = RamProject::project( uuid );
    if (!project) project = new RamProject(
                newP.value("shortName").toString(),
                newP.value("name").toString(),
                uuid
                );

    project->setWidth( newP.value("width").toInt());
    project->setHeight( newP.value("height").toInt());
    project->setFramerate( newP.value("framerate").toDouble());
    project->setFolderPath( newP.value("folderPath").toString());
    project->setComment( newP.value("comment").toString());

    if (!init)
    {
        m_pm->setMaximum(7);
        gotSteps( newP.value("steps").toArray(), project);        
        gotPipeFiles( newP.value("pipeFiles").toArray(), project );
        gotPipes( newP.value("pipes").toArray(), project);
        gotAssetGroups( newP.value("assetGroups").toArray(), project);
        gotAssets( newP.value("assets").toArray(), project);
        gotSequences( newP.value("sequences").toArray(), project);
        gotShots( newP.value("shots").toArray(), project);
    }

    m_ram->projects()->append(project);

    return uuid;
}

void RamLoader::gotTemplateSteps(QJsonArray steps)
{
    DBISuspender s;

    m_pm->setText("Loading template steps...");
    m_pm->increment();
    qDebug() << "Loading template steps";

    RamObjectList *tss = m_ram->templateSteps();

    QStringList uuids;
    // Update states
    for (int j = 0; j < steps.count(); j++)
        uuids << gotTemplateStep( steps.at(j).toObject() );

    // Remove deleted states
    m_pm->setText("Cleaning template steps...");
    qDebug() << "Cleaning template steps";

    for (int i = tss->count() - 1; i >= 0; i--)
    {
        RamObject *ts = tss->at(i);
        if (!uuids.contains(ts->uuid()))
            ts->remove();
    }

    // sort the states
    tss->sort();
}

QString RamLoader::gotTemplateStep(QJsonObject newS)
{
    DBISuspender s;
    QString uuid = newS.value("uuid").toString();

    RamStep *step = RamStep::step( uuid );
    if(!step) step = new RamStep(
                newS.value("shortName").toString(),
                newS.value("name").toString(),
                uuid
                );
    step->setType( newS.value("type").toString());
    step->setComment( newS.value("comment").toString());

    m_ram->templateSteps()->append(step);

    return uuid;
}

void RamLoader::gotTemplateAssetGroups(QJsonArray assetGroups)
{
    DBISuspender s;

    m_pm->setText("Loading template asset groups...");
    m_pm->increment();
    qDebug() << "Loading template asset groups";

    RamObjectList *tags = m_ram->templateAssetGroups();

    QStringList uuids;
    // Update states
    for (int j = 0; j < assetGroups.count(); j++)
        uuids << gotTemplateAssetGroup( assetGroups.at(j).toObject() );

    // Remove deleted states
    m_pm->setText("Cleaning template asset groups...");
    qDebug() << "Cleaning template asset groups";

    for (int i = tags->count() - 1; i >= 0; i--)
    {
        RamObject *tag = tags->at(i);
        if (!uuids.contains(tag->uuid()))
            tag->remove();
    }

    // sort the states
    tags->sort();
}

QString RamLoader::gotTemplateAssetGroup(QJsonObject newAG)
{
    DBISuspender s;
    QString uuid = newAG.value("uuid").toString();

    RamAssetGroup *assetGroup = RamAssetGroup::assetGroup( uuid );
    if(!assetGroup) assetGroup = new RamAssetGroup(
                newAG.value("shortName").toString(),
                newAG.value("name").toString(),
                uuid
                );
    assetGroup->setComment( newAG.value("comment").toString());
    m_ram->templateAssetGroups()->append(assetGroup);

    return uuid;
}

void RamLoader::gotStates(QJsonArray states)
{
    DBISuspender s;

    m_pm->setText("Loading states...");
    m_pm->increment();
    qDebug() << "Loading states";

    RamObjectList *stts = m_ram->states();

    QStringList uuids;
    // Update states
    for (int j = 0; j < states.count(); j++)
        uuids << gotState( states.at(j).toObject() );

    // Remove deleted states
    m_pm->setText("Cleaning states...");
    qDebug() << "Cleaning states";

    for (int i = stts->count() - 1; i >= 0; i--)
    {
        RamObject *st = stts->at(i);
        if (!uuids.contains(st->uuid()))
            st->remove();
    }

    // sort the states
    stts->sort();
}

QString RamLoader::gotState(QJsonObject newS)
{
    DBISuspender s;
    QString uuid = newS.value("uuid").toString();

    RamState *state = RamState::state( uuid );
    if (!state) state = new RamState(
                newS.value("shortName").toString(),
                newS.value("name").toString(),
                uuid
                );
    state->setColor( QColor( newS.value("color").toString()) );
    state->setCompletionRatio( newS.value("completionRatio").toInt() );
    state->setComment( newS.value("comment").toString());

    m_ram->states()->append(state);

    return uuid;
}

void RamLoader::gotFileTypes(QJsonArray fileTypes)
{
    DBISuspender s;

    m_pm->setText("Loading file types...");
    m_pm->increment();
    qDebug() << "Loading file types";

    RamObjectList *fts = m_ram->fileTypes();

    QStringList uuids;
    // Update apps
    for (int j = 0; j < fileTypes.count(); j++)
        uuids << gotFileType( fileTypes.at(j).toObject() );

    // Remove deleted apps
    m_pm->setText("Cleaning file types...");
    qDebug() << "Cleaning file types";

    for (int i = fts->count() - 1; i >= 0; i--)
    {
        RamObject *ft = fts->at(i);
        if (!uuids.contains(ft->uuid()))
            ft->remove();
    }

    // sort the steps
    fts->sort();
}

QString RamLoader::gotFileType(QJsonObject newFt)
{
    DBISuspender s;
    QString uuid = newFt.value("uuid").toString();

    RamFileType *fileType = RamFileType::fileType( uuid );
    if (!fileType) fileType = new RamFileType(
                newFt.value("shortName").toString(),
                newFt.value("name").toString(),
                newFt.value("extensions").toString(),
                uuid
                );

    fileType->setPreviewable( newFt.value("previewable").toInt() != 0 );
    fileType->setComment( newFt.value("comment").toString());

    m_ram->fileTypes()->append(fileType);

    return uuid;
}

void RamLoader::gotApplications(QJsonArray applications)
{
    DBISuspender s;

    m_pm->setText("Loading applications...");
    m_pm->increment();
    qDebug() << "Loading Applications";

    RamObjectList *apps = m_ram->applications();

    QStringList uuids;
    // Update apps
    for (int j = 0; j < applications.count(); j++)
        uuids << gotApplication( applications.at(j).toObject() );

    // Remove deleted apps
    m_pm->setText("Cleaning applications...");
    qDebug() << "Cleaning applications";

    for (int i = apps->count() - 1; i >= 0; i--)
    {
        RamObject *existingApp = apps->at(i);
        if (!uuids.contains(existingApp->uuid()))
            existingApp->remove();
    }

    // sort the steps
    apps->sort();
}

QString RamLoader::gotApplication(QJsonObject newA)
{
    DBISuspender s;
    QString uuid = newA.value("uuid").toString();

    RamApplication *application = RamApplication::application( uuid );
    if (!application) application = new RamApplication(
                newA.value("shortName").toString(),
                newA.value("name").toString(),
                newA.value("executableFilePath").toString(),
                newA.value("uuid").toString()
                );

    application->setComment( newA.value("comment").toString());

    application->importFileTypes()->clear();
    application->exportFileTypes()->clear();
    application->nativeFileTypes()->clear();

    foreach( QJsonValue ft, newA.value("fileTypes").toArray())
    {
        QJsonObject fileT = ft.toObject();
        if ( fileT.value("type").toString() == "import" )
            application->importFileTypes()->append( RamFileType::fileType( fileT.value("uuid").toString() ) );
        else if (fileT.value("type").toString() == "export" )
            application->exportFileTypes()->append( RamFileType::fileType( fileT.value("uuid").toString() ) );
        else
            application->nativeFileTypes()->append( RamFileType::fileType( fileT.value("uuid").toString() ) );
    }

    m_ram->applications()->append(application);
    return uuid;
}

void RamLoader::gotSteps(QJsonArray steps, RamProject *project)
{
    DBISuspender s;

    m_pm->setText("Loading steps...");
    m_pm->increment();
    qDebug() << "Loading Steps";

    RamObjectList *projectSteps = project->steps();

    QStringList uuids;
    // Update steps
    for (int j = 0; j < steps.count(); j++)
        uuids << gotStep( steps.at(j).toObject(), project );

    // Remove deleted shots
    m_pm->setText("Cleaning steps...");
    qDebug() << "Cleaning Steps";

    for (int i = projectSteps->count() - 1; i >= 0; i--)
    {
        RamObject *existingStep = projectSteps->at(i);
        if (!uuids.contains(existingStep->uuid()))
            existingStep->remove();
    }

    // sort the steps
    projectSteps->sort();
}

QString RamLoader::gotStep(QJsonObject newS, RamProject *project)
{
    DBISuspender s;
    QString uuid = newS.value("uuid").toString();

    RamStep *step = RamStep::step( uuid );
    if (!step ) step = new RamStep(
                newS.value("shortName").toString(),
                newS.value("name").toString(),
                project,
                uuid
                );
    step->setType( newS.value("type").toString());
    step->setOrder( newS.value("order").toInt() );
    step->setComment( newS.value("comment").toString());

    step->users()->clear();
    step->applications()->clear();

    foreach( QJsonValue u, newS.value("users").toArray())
        step->users()->append( RamUser::user( u.toString() ) );


    foreach(QJsonValue a, newS.value("applications").toArray())
        step->applications()->append( RamApplication::application(a.toString()) );

    project->steps()->append(step);

    return uuid;
}

void RamLoader::gotAssetGroups(QJsonArray assetGroups, RamProject *project)
{
    DBISuspender s;

    m_pm->increment();
    m_pm->setText("Loading asset groups...");
    qDebug() << "Loading Asset groups";

    QStringList uuids;
    // Update asset groups
    for (int j = 0; j < assetGroups.count(); j++)
        uuids << gotAssetGroup( assetGroups.at(j).toObject(), project );

    // Remove deleted asset groups
    m_pm->setText("Cleaning asset groups...");
    qDebug() << "Cleaning Asset groups";
    RamObjectList *projectAssetGroups = project->assetGroups();
    for (int i = projectAssetGroups->count() - 1; i >= 0; i--)
    {
        RamObject *existingAssetGroup = projectAssetGroups->at(i);
        if (!uuids.contains(existingAssetGroup->uuid()))
            existingAssetGroup->remove();
    }

    // sort the sequences
    projectAssetGroups->sort();
}

QString RamLoader::gotAssetGroup(QJsonObject newAG, RamProject *project)
{
    DBISuspender s;
    QString uuid = newAG.value("uuid").toString();

    RamAssetGroup *assetGroup = RamAssetGroup::assetGroup( uuid );
    if (!assetGroup) assetGroup = new RamAssetGroup(
                newAG.value("shortName").toString(),
                project,
                newAG.value("name").toString(),
                uuid
                );
    assetGroup->setComment( newAG.value("comment").toString());

    project->assetGroups()->append(assetGroup);

    return uuid;
}

void RamLoader::gotAssets(QJsonArray assets, RamProject *project)
{
    DBISuspender s;

    m_pm->increment();
    m_pm->setText("Loading assets...");
    qDebug() << "Loading assets";

    QStringList uuids;

    // Update assets
    for (int j = 0; j < assets.count(); j++)
        uuids << gotAsset( assets.at(j).toObject(), project );

    // Remove deleted assets
    m_pm->setText("Cleaning assets...");
    qDebug() << "Cleaning assets";
    for (int i = project->assets()->count() - 1; i >= 0; i--)
    {
        RamObject *obj = project->assets()->at(i);
        if (!uuids.contains(obj->uuid()))
        {
            obj->remove();
        }
    }

    // sort the asset group
    project->assets()->sort();
}

QString RamLoader::gotAsset(QJsonObject newA, RamProject *project)
{
    DBISuspender s;
    QString uuid = newA.value("uuid").toString();

    RamAssetGroup *ag = RamAssetGroup::assetGroup( newA.value("assetGroupUuid").toString() );
    if (!ag) return "";

    QString shortName = newA.value("shortName").toString();
    QString name = newA.value("name").toString();

    RamAsset *asset = RamAsset::asset( uuid );
    if (!asset) asset = new RamAsset(
                shortName,
                ag,
                name,
                uuid
                );

    asset->setShortName(shortName);
    asset->setName(name);
    asset->setAssetGroup(ag);
    asset->setTags( newA.value("tags").toString());
    asset->setComment( newA.value("comment").toString());
    gotStatusHistory( newA.value("statusHistory").toArray(), asset);

    project->assets()->append(asset);

    return uuid;
}

void RamLoader::gotSequences(QJsonArray sequences, RamProject *project)
{
    DBISuspender s;

    m_pm->increment();
    m_pm->setText("Loading sequences...");
    qDebug() << "Loading sequences";

    QStringList uuids;
    // Update sequences
    for (int j = 0; j < sequences.count(); j++)
        uuids << gotSequence( sequences.at(j).toObject(), project );

    // Remove deleted sequences
    m_pm->setText("Cleaning sequences...");
    qDebug() << "Cleaning sequences";
    RamObjectList *projectSequences = project->sequences();
    for (int i = projectSequences->count() - 1; i >= 0; i--)
    {
        RamObject *existingSequence = projectSequences->at(i);
        if (!uuids.contains(existingSequence->uuid()))
        {
            existingSequence->remove();
        }
    }

    // sort the sequences
    projectSequences->sort();
}

QString RamLoader::gotSequence(QJsonObject newS, RamProject *project)
{
    DBISuspender s;
    QString uuid = newS.value("uuid").toString();

    RamSequence *sequence = RamSequence::sequence( uuid );
    if (!sequence) sequence = new RamSequence(
                newS.value("shortName").toString(),
                project,
                newS.value("name").toString(),
                uuid
                );

    sequence->setComment( newS.value("comment").toString());

    project->sequences()->append(sequence);

    return uuid;
}

void RamLoader::gotShots(QJsonArray shots, RamProject *project)
{
    DBISuspender s;

    m_pm->increment();
    m_pm->setText("Loading shots...");
    qDebug() << "Loading shots";

    QStringList uuids;
    // Update shots
    for (int j = 0; j < shots.count(); j++)
        uuids << gotShot( shots.at(j).toObject(), project );

    // Remove deleted shots
    m_pm->setText("Cleaning shots...");
    qDebug() << "Cleaning shots";
    for (int i = project->shots()->count() - 1; i >= 0; i--)
    {
        RamObject *existingShot = project->shots()->at(i);
        if (!uuids.contains(existingShot->uuid()))
            existingShot->remove();
    }

    // sort the shots
    project->shots()->sort();
}

QString RamLoader::gotShot(QJsonObject newS, RamProject *project)
{
    DBISuspender s;
    QString uuid = newS.value("uuid").toString();

    RamSequence *seq = RamSequence::sequence( newS.value("sequenceUuid").toString() );
    if (!seq) return "";

    QString shortName = newS.value("shortName").toString();
    QString name = newS.value("name").toString();

    RamShot *shot = RamShot::shot( uuid );
    if (!shot) shot = new RamShot(
                shortName,
                seq,
                name,
                uuid
                );
    shot->setShortName(shortName);
    shot->setName(name);
    shot->setSequence(seq);
    shot->setDuration( newS.value("duration").toDouble() );
    shot->setOrder( newS.value("order").toInt() );
    shot->setComment( newS.value("comment").toString());
    gotStatusHistory( newS.value("statusHistory").toArray(), shot);

    // Assign assets
    QJsonArray assts = newS.value("assetUuids").toArray();
    shot->assets()->clear();
    for (int i = 0; i < assts.count(); i++)
    {
        RamObject *assetObj = RamAsset::asset( assts.at(i).toString() );
        if (!assetObj) continue;
        shot->assets()->append(assetObj);
    }

    project->shots()->append(shot);

    return uuid;
}

void RamLoader::gotStatusHistory(QJsonArray statusHistory, RamItem *item)
{
    DBISuspender s;

    QStringList uuids;
    // Update status
    for (int j = 0; j < statusHistory.count(); j++)
        uuids << gotStatus( statusHistory.at(j).toObject(), item );

    // Remove deleted status
    QMap<QString, RamStepStatusHistory *> history = item->statusHistory();
    QMapIterator<QString, RamStepStatusHistory*> i(history);
    while(i.hasNext())
    {
        i.next();
        RamStepStatusHistory *stepHistory = i.value();
        for (int j = 0; j < stepHistory->count(); j++)
        {
            RamObject *obj = stepHistory->at(j);
            if (!uuids.contains( obj->uuid() ))
                obj->remove();
        }
        stepHistory->sort();
    }
}

QString RamLoader::gotStatus(QJsonObject newS, RamItem *item)
{
    DBISuspender s;
    QString uuid = newS.value("uuid").toString();

    RamUser *user = RamUser::user( newS.value("userUuid").toString( ) );
    RamState *state = RamState::state( newS.value("stateUuid").toString( ) );
    RamStep *step = RamStep::step( newS.value("stepUuid").toString( ) );
    if ( !user || !state || !step ) return "";

    RamStatus *status = RamStatus::status( uuid );
    if (!status) status = new RamStatus(user, state, step, item, uuid);

    status->setCompletionRatio( newS.value("completionRatio").toInt( ) );
    status->setComment( newS.value("comment").toString( ) );
    status->setVersion( newS.value("version").toInt( ) );
    status->setDate( QDateTime::fromString( newS.value("date").toString(), "yyyy-MM-dd hh:mm:ss"));
    status->assignUser( RamUser::user( newS.value("assignedUserUuid").toString( )));
    status->setPublished( newS.value("published").toInt() == 1);
    status->setTimeSpent( newS.value("timeSpent").toInt() );

    item->addStatus(status);

    return uuid;
}

void RamLoader::gotPipes(QJsonArray pipes, RamProject *project)
{
    DBISuspender s;

    m_pm->increment();
    m_pm->setText("Loading pipes...");
    qDebug() << "Loading pipes";

    QStringList uuids;
    // Update pipes
    for (int j = 0; j < pipes.count(); j++)
        uuids << gotPipe( pipes.at(j).toObject(), project );

    // Remove deleted pipes
    m_pm->setText("Cleaning pipes...");
    qDebug() << "Cleaning pipes";
    RamObjectList *projectPipeline = project->pipeline();
    for (int i = projectPipeline->count() - 1; i >= 0; i--)
    {
        RamObject *existingPipe = projectPipeline->at(i);
        if (!uuids.contains(existingPipe->uuid()))
        {
            existingPipe->remove();
        }
    }
}

QString RamLoader::gotPipe(QJsonObject newP, RamProject *project)
{
    DBISuspender s;
    QString uuid = newP.value("uuid").toString();

    RamStep *inputStep = RamStep::step( newP.value("inputStepUuid").toString( ) );
    RamStep *outputStep = RamStep::step( newP.value("outputStepUuid").toString( ) );

    RamPipe *pipe = RamPipe::pipe(uuid);
    if (!pipe) pipe = new RamPipe(outputStep, inputStep, uuid);

    QJsonArray newPFs = newP.value("pipeFiles").toArray();
    for (int j = 0; j < newPFs.count(); j++)
    {
        RamPipeFile *pf = RamPipeFile::pipeFile( newPFs.at(j).toString() );
        if (pf) pipe->pipeFiles()->append(pf);
    }

    project->pipeline()->append( pipe );

    return uuid;
}

void RamLoader::gotPipeFiles(QJsonArray pipeFiles, RamProject *project)
{
    DBISuspender s;

    m_pm->increment();
    m_pm->setText("Loading pipe files...");
    qDebug() << "Loading pipe files";

    QStringList uuids;
    // Update pipes
    for (int j = 0; j < pipeFiles.count(); j++)
        uuids << gotPipeFile( pipeFiles.at(j).toObject(), project );

    // Remove deleted pipes
    m_pm->setText("Cleaning pipe files...");
    qDebug() << "Cleaning pipe files";
    RamObjectList *projectPipeFiles = project->pipeFiles();
    for (int i = projectPipeFiles->count() - 1; i >= 0; i--)
    {
        RamObject *existingPipeFile = projectPipeFiles->at(i);
        if (!uuids.contains(existingPipeFile->uuid()))
        {
            existingPipeFile->remove();
        }
    }
}

QString RamLoader::gotPipeFile(QJsonObject newPF, RamProject* project)
{
    DBISuspender s;

    QString uuid = newPF.value("uuid").toString();

    // Get existing one if any, otherwise create it
    RamPipeFile *pipeFile = RamPipeFile::pipeFile(uuid);
    if (!pipeFile) pipeFile = new RamPipeFile(uuid, this);

    pipeFile->setShortName( newPF.value("shortName").toString() );
    pipeFile->setProject( project );
    pipeFile->setComment( newPF.value("comment").toString());

    RamFileType *ft = RamFileType::fileType( newPF.value("fileTypeUuid").toString() );
    if (ft) pipeFile->setFileType(ft);

    project->pipeFiles()->append(pipeFile);

    return uuid;
}
