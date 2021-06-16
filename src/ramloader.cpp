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
        m_pm->setTitle("Loading project.");
        QString uuid = gotProject( m_data.value("content").toObject(), false);
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

    m_pm->addToMaximum( users.count() );
    m_pm->setText("Loading users...");

    RamObjectList *_users = m_ram->users();

    // loop through existing users to update them
    for (int i = _users->count() - 1; i >= 0; i--)
    {
        m_pm->increment();

        RamUser *existingUser = (RamUser*)_users->at(i);
        // loop through new users to update
        bool found = false;
        for (int j = users.count() - 1; j >= 0; j--)
        {
            QJsonObject newUser = users[j].toObject();
            QString uuid = newUser.value("uuid").toString();
            // Found, update
            if (uuid == existingUser->uuid())
            {
                found = true;

                existingUser->setName( newUser.value("name").toString());
                existingUser->setShortName( newUser.value("shortName").toString());
                existingUser->setFolderPath( newUser.value("folderPath").toString());
                existingUser->setRole( newUser.value("role").toString("standard") );

                //remove from new users
                users.removeAt(j);
                break;
            }
        }
        // Not found, remove from existing
        if (!found)
        {
            RamObject *u = _users->takeAt(i);
            u->remove();
        }
    }

    // loop through remaining new users to add them
    for (int i = 0; i < users.count(); i++)
    {
        m_pm->increment();

        QJsonObject u = users[i].toObject();
        RamUser *user = new RamUser(
                    u.value("shortName").toString(),
                    u.value("name").toString(),
                    u.value("uuid").toString()
                    );
        user->setFolderPath(u.value("folderPath").toString());
        user->setRole( u.value("role").toString("standard") );

        _users->append( user );
    }

    // Set the current and ramses user
    for (int i = 0; i < _users->count(); i++)
    {
        if (_users->at(i)->shortName() == m_currentUserShortName)
        {
            m_ram->setCurrentUser( qobject_cast<RamUser*>( _users->at(i) ));
            return;
        }
    }

    // Not found
    m_ram->setCurrentUser(nullptr);
}

void RamLoader::gotProjects(QJsonArray projects, bool init)
{
    DBISuspender s;

    m_pm->addToMaximum(projects.count());
    m_pm->setText("Loading projects...");

    QStringList uuids;
    // Update projects
    for (int j = 0; j < projects.count(); j++)
    {
        m_pm->increment();
        uuids << gotProject( projects.at(j).toObject(), init );
    }

    RamObjectList *_projects = m_ram->projects();

    // Remove deleted projects
    for (int i = _projects->count() - 1; i >= 0; i--)
    {
        RamObject *existingProject = _projects->at(i);
        if (!uuids.contains(existingProject->uuid()))
        {
            RamObject *p = _projects->takeAt(i);
            p->remove();
        }
    }
}

QString RamLoader::gotProject(QJsonObject newP, bool init)
{
    DBISuspender s;

    QString uuid = newP.value("uuid").toString();

    RamObjectList *_projects = m_ram->projects();

    // loop through existing projects to update them
    for (int i = _projects->count() - 1; i >= 0; i--)
    {
        RamProject *existingProject = (RamProject*)_projects->at(i);

        if (uuid == existingProject->uuid())
        {
            qDebug() << "UPDATING PROJECT: " + existingProject->shortName();

            //Emit just one signal
            existingProject->setName( newP.value("name").toString());
            existingProject->setShortName( newP.value("shortName").toString());
            existingProject->setWidth( newP.value("width").toInt());
            existingProject->setHeight( newP.value("height").toInt());
            existingProject->setFramerate( newP.value("framerate").toDouble());
            existingProject->setFolderPath( newP.value("folderPath").toString());

            if (!init)
            {
                gotSteps( newP.value("steps").toArray(), existingProject);
                gotAssetGroups( newP.value("assetGroups").toArray(), existingProject);
                gotSequences( newP.value("sequences").toArray(), existingProject);
                gotPipeFiles( newP.value("pipeFiles").toArray(), existingProject );
                gotPipes( newP.value("pipes").toArray(), existingProject);
            }

            qDebug() << "> Project updated";
            return uuid;
        }
    }

    // not existing, let's create it
    qDebug() << "CREATING PROJECT: " +newP.value("shortName").toString();
    RamProject *project = new RamProject(
                newP.value("shortName").toString(),
                newP.value("name").toString(),
                newP.value("uuid").toString()
                );
    project->setWidth( newP.value("width").toInt());
    project->setHeight( newP.value("height").toInt());
    project->setFramerate( newP.value("framerate").toDouble());
    project->setFolderPath( newP.value("folderPath").toString());

    // Add steps, assets, sequences, pipes...
    if (!init)
    {
        gotSteps( newP.value("steps").toArray(), project);
        gotAssetGroups( newP.value("assetGroups").toArray(), project);
        gotSequences( newP.value("sequences").toArray(), project);
        gotPipeFiles( newP.value("pipeFiles").toArray(), project );
        gotPipes( newP.value("pipes").toArray(), project);
    }

    _projects->append(project);

    qDebug() << "> Project created";

    return uuid;
}

void RamLoader::gotTemplateSteps(QJsonArray steps)
{
    DBISuspender s;

    m_pm->addToMaximum(steps.count());
    m_pm->setText("Loading template steps...");

    RamObjectList *_templateSteps = m_ram->templateSteps();

    // loop through existing steps to update them
    for (int i = _templateSteps->count() - 1; i >= 0; i--)
    {
        m_pm->increment();

        RamStep *existingStep = (RamStep*)_templateSteps->at(i);
        // loop through new steps to update
        bool found = false;
        for (int j = steps.count() - 1; j >= 0; j--)
        {
            QJsonObject newStep = steps[j].toObject();
            QString uuid = newStep.value("uuid").toString();
            // Found, update
            if (uuid == existingStep->uuid())
            {
                found = true;

                existingStep->setName( newStep.value("name").toString());
                existingStep->setShortName( newStep.value("shortName").toString());
                existingStep->setType(newStep.value("type").toString());
                //remove from new projects
                steps.removeAt(j);
                break;
            }
        }
        // Not found, remove from existing
        if (!found)
        {
            RamObject *s = _templateSteps->takeAt(i);
            s->remove();
        }
    }

    // loop through remaining new projects to add them
    for (int i = 0; i < steps.count(); i++)
    {
        m_pm->increment();

        QJsonObject s = steps[i].toObject();
        RamStep *step = new RamStep(
                    s.value("shortName").toString(),
                    s.value("name").toString(),
                    true,
                    s.value("uuid").toString()
                    );
        step->setType( s.value("type").toString());

        _templateSteps->append(step);
    }
}

void RamLoader::gotTemplateAssetGroups(QJsonArray assetGroups)
{
    DBISuspender s;

    m_pm->addToMaximum(assetGroups.count());
    m_pm->setText("Loading template asset groups...");

    RamObjectList *_templateAssetGroups = m_ram->templateAssetGroups();

    // loop through existing asset groups to update them
    for (int i = _templateAssetGroups->count() - 1; i >= 0; i--)
    {
        m_pm->increment();
        RamAssetGroup *existingAssetGroup = (RamAssetGroup*)_templateAssetGroups->at(i);
        // loop through new steps to update
        bool found = false;
        for (int j = assetGroups.count() - 1; j >= 0; j--)
        {
            QJsonObject newAssetGroup = assetGroups[j].toObject();
            QString uuid = newAssetGroup.value("uuid").toString();
            // Found, update
            if (uuid == existingAssetGroup->uuid())
            {
                found = true;

                existingAssetGroup->setName( newAssetGroup.value("name").toString());
                existingAssetGroup->setShortName( newAssetGroup.value("shortName").toString());

                //remove from new projects
                assetGroups.removeAt(j);
                break;
            }
        }
        // Not found, remove from existing
        if (!found)
        {
            RamObject *ag = _templateAssetGroups->takeAt(i);
            ag->remove();
        }
    }

    // loop through remaining new projects to add them
    for (int i = 0; i < assetGroups.count(); i++)
    {
        m_pm->increment();
        QJsonObject ag = assetGroups[i].toObject();
        RamAssetGroup *assetGroup = new RamAssetGroup(
                    ag.value("shortName").toString(),
                    ag.value("name").toString(),
                    true,
                    ag.value("uuid").toString()
                    );

        _templateAssetGroups->append(assetGroup);
    }
}

void RamLoader::gotStates(QJsonArray states)
{
    DBISuspender s;

    m_pm->addToMaximum(states.count());
    m_pm->setText("Loading states...");

    RamObjectList *_states = m_ram->states();

    // loop through existing steps to update them
    for (int i = _states->count() - 1; i >= 0; i--)
    {
        m_pm->increment();
        RamState *existingState = (RamState*)_states->at(i);
        // loop through new steps to update
        bool found = false;
        for (int j = states.count() - 1; j >= 0; j--)
        {
            QJsonObject newState = states[j].toObject();
            QString uuid = newState.value("uuid").toString();
            // Found, update
            if (uuid == existingState->uuid())
            {
                found = true;

                existingState->setName( newState.value("name").toString());
                existingState->setShortName( newState.value("shortName").toString());
                existingState->setColor( QColor(newState.value("color").toString()));
                existingState->setCompletionRatio(newState.value("completionRatio").toInt());
                //remove from new projects
                states.removeAt(j);
                break;
            }
        }
        // Not found, remove from existing
        if (!found)
        {
            RamState *s = (RamState*)_states->takeAt(i);
            s->remove();
        }
    }

    // loop through remaining new states to add them
    for (int i = 0; i < states.count(); i++)
    {
        m_pm->increment();
        QJsonObject s = states[i].toObject();
        RamState *state = new RamState(
                    s.value("shortName").toString(),
                    s.value("name").toString(),
                    s.value("uuid").toString()
                    );
        state->setColor( QColor( s.value("color").toString()) );
        state->setCompletionRatio( s.value("completionRatio").toInt() );

        _states->append(state);
    }
    _states->sort();
}

void RamLoader::gotFileTypes(QJsonArray fileTypes)
{
    DBISuspender s;

    m_pm->addToMaximum(fileTypes.count());
    m_pm->setText("Loading file types...");

    RamObjectList *_fileTypes = m_ram->fileTypes();

    // loop through existing file types to update them
    for (int i = _fileTypes->count() - 1; i >= 0; i--)
    {
        m_pm->increment();
        RamFileType *existingFileType = (RamFileType*)_fileTypes->at(i);
        // loop through new file types to update
        bool found = false;
        for (int j = fileTypes.count() - 1; j >= 0; j--)
        {
            QJsonObject newFileType = fileTypes[j].toObject();
            QString uuid = newFileType.value("uuid").toString();
            // Found, update
            if (uuid == existingFileType->uuid())
            {
                found = true;

                existingFileType->setName( newFileType.value("name").toString());
                existingFileType->setShortName( newFileType.value("shortName").toString() );
                existingFileType->setPreviewable( newFileType.value("previewable").toInt() != 0 );
                existingFileType->setExtensions(newFileType.value("extensions").toString());
                //remove from new projects
                fileTypes.removeAt(j);
                break;
            }
        }
        // Not found, remove from existing
        if (!found)
        {
            RamObject *ft = _fileTypes->takeAt(i);
            ft->remove();
        }
    }

    // loop through remaining new file types to add them
    for (int i = 0; i < fileTypes.count(); i++)
    {
        m_pm->increment();
        QJsonObject ft = fileTypes[i].toObject();
        RamFileType *fileType = new RamFileType(
                    ft.value("shortName").toString(),
                    ft.value("name").toString(),
                    ft.value("extensions").toString(),
                    ft.value("uuid").toString()
                    );
        fileType->setPreviewable( ft.value("previewable").toInt() != 0 );

        _fileTypes->append(fileType);
    }
}

void RamLoader::gotApplications(QJsonArray applications)
{
    DBISuspender s;

    m_pm->addToMaximum(applications.count());
    m_pm->setText("Loading applications...");

    RamObjectList *_applications = m_ram->applications();

    // loop through existing applications to update them
    for (int i = _applications->count() - 1; i >= 0; i--)
    {
        m_pm->increment();
        RamApplication *existingApplication = (RamApplication*)_applications->at(i);
        // loop through new steps to update
        bool found = false;
        for (int j = applications.count() - 1; j >= 0; j--)
        {
            QJsonObject newApplication = applications[j].toObject();
            QString uuid = newApplication.value("uuid").toString();
            // Found, update
            if (uuid == existingApplication->uuid())
            {
                found = true;
                existingApplication->setName( newApplication.value("name").toString());
                existingApplication->setShortName( newApplication.value("shortName").toString());
                existingApplication->setExecutableFilePath(newApplication.value("executableFilePath").toString());
                RamObjectList *importFt = existingApplication->importFileTypes();
                RamObjectList *exportFt = existingApplication->exportFileTypes();
                RamObjectList *nativeFt = existingApplication->nativeFileTypes();
                QStringList importuuids;
                QStringList exportuuids;
                QStringList nativeuuids;
                foreach( QJsonValue ft, newApplication.value("fileTypes").toArray())
                {
                    QJsonObject fileT = ft.toObject();
                    QString uuid = fileT.value("uuid").toString();
                    RamObject *fileTypeObj = m_ram->fileTypes()->fromUuid( uuid );
                    // Add if new
                    if (fileTypeObj)
                    {
                        QString t = fileT.value("type").toString();
                        if (t == "import")
                        {
                           importFt->append( fileTypeObj );
                           importuuids << uuid;
                        }
                        if (t == "export")
                        {
                            exportFt->append( fileTypeObj );
                            exportuuids << uuid;
                        }
                        if (t == "native")
                        {
                            nativeFt->append( fileTypeObj );
                            nativeuuids << uuid;
                        }
                    }
                }
                // Remove other
                for (int i = importFt->count() - 1; i >= 0 ; i--)
                {
                    if (!importuuids.contains( importFt->at(i)->uuid() )) importFt->removeAt(i);
                }
                for (int i = exportFt->count() - 1; i >= 0 ; i--)
                {
                    if (!importuuids.contains( exportFt->at(i)->uuid() )) exportFt->removeAt(i);
                }
                for (int i = nativeFt->count() - 1; i >= 0 ; i--)
                {
                    if (!importuuids.contains( nativeFt->at(i)->uuid() )) nativeFt->removeAt(i);
                }

                //remove from new projects
                applications.removeAt(j);
                break;
            }
        }
        // Not found, remove from existing
        if (!found)
        {
            RamObject *a = _applications->takeAt(i);
            a->remove();
        }
    }

    // loop through remaining new applications to add them
    for (int i = 0; i < applications.count(); i++)
    {
        m_pm->increment();
        QJsonObject a = applications[i].toObject();
        RamApplication *app = new RamApplication(
                    a.value("shortName").toString(),
                    a.value("name").toString(),
                    a.value("executableFilePath").toString(),
                    a.value("uuid").toString()
                    );

        foreach( QJsonValue ft, a.value("fileTypes").toArray())
        {
            QJsonObject fileT = ft.toObject();
            if ( fileT.value("type").toString() == "import" )
                app->importFileTypes()->append( m_ram->fileTypes()->fromUuid( fileT.value("uuid").toString() ) );
            else if (fileT.value("type").toString() == "export" )
                app->exportFileTypes()->append( m_ram->fileTypes()->fromUuid( fileT.value("uuid").toString() ) );
            else
                app->nativeFileTypes()->append( m_ram->fileTypes()->fromUuid( fileT.value("uuid").toString() ) );
        }

        _applications->append(app);
    }
}

void RamLoader::gotSteps(QJsonArray steps, RamProject *project)
{
    DBISuspender s;

    m_pm->addToMaximum(steps.count());
    m_pm->setText("Loading steps...");
    qDebug() << "Loading Steps";

    RamObjectList *projectSteps = project->steps();



    QStringList uuids;
    // Update steps
    for (int j = 0; j < steps.count(); j++)
    {
        m_pm->increment();
        uuids << gotStep( steps.at(j).toObject(), project );
    }

    // Remove deleted shots
    for (int i = projectSteps->count() - 1; i >= 0; i--)
    {
        RamObject *existingStep = projectSteps->at(i);
        if (!uuids.contains(existingStep->uuid()))
        {
            existingStep->remove();
        }
    }

    // sort the steps
    projectSteps->sort();
}

QString RamLoader::gotStep(QJsonObject newS, RamProject *project)
{
    DBISuspender s;
    QString uuid = newS.value("uuid").toString();

    RamObjectList *projectSteps = project->steps();

    // loop through existing steps to update them
    for (int i = projectSteps->count() - 1; i >= 0; i--)
    {
        RamStep *existingStep = qobject_cast<RamStep*>( projectSteps->at(i) );

        if (uuid == existingStep->uuid())
        {
            existingStep->setName( newS.value("name").toString());
            existingStep->setShortName( newS.value("shortName").toString());
            existingStep->setOrder( newS.value("order").toInt());
            existingStep->users()->clear();
            existingStep->setType(newS.value("type").toString());

            foreach( QJsonValue u, newS.value("users").toArray())
                existingStep->users()->append( m_ram->users()->fromUuid( u.toString() ) );

            foreach(QJsonValue a, newS.value("applications").toArray())
                existingStep->applications()->append( m_ram->applications()->fromUuid(a.toString()));

            return uuid;
        }
    }

    RamStep *step = new RamStep(
                newS.value("shortName").toString(),
                newS.value("name").toString(),
                false,
                newS.value("uuid").toString()
                );
    step->setType( newS.value("type").toString());
    step->setOrder( newS.value("order").toInt() );
    step->setProjectUuid( newS.value("projectUuid").toString());

    foreach( QJsonValue u, newS.value("users").toArray())
        step->users()->append( m_ram->users()->fromUuid(u.toString()) );

    foreach(QJsonValue a, newS.value("applications").toArray())
        step->applications()->append( m_ram->applications()->fromUuid(a.toString()));

    projectSteps->append(step);

    return uuid;
}

void RamLoader::gotAssetGroups(QJsonArray assetGroups, RamProject *project)
{
    DBISuspender s;

    m_pm->addToMaximum(assetGroups.count());
    m_pm->setText("Loading asset groups...");
    qDebug() << "Loading Asset groups";

    QStringList uuids;
    // Update asset groups
    for (int j = 0; j < assetGroups.count(); j++)
    {
        m_pm->increment();
        uuids << gotAssetGroup( assetGroups.at(j).toObject(), project );
    }

    // Remove deleted asset groups
    RamObjectUberList *projectAssetGroups = project->assetGroups();
    for (int i = projectAssetGroups->count() - 1; i >= 0; i--)
    {
        RamObject *existingAssetGroup = projectAssetGroups->at(i);
        if (!uuids.contains(existingAssetGroup->uuid()))
        {
            existingAssetGroup->remove();
        }
    }

    // sort the sequences
    projectAssetGroups->sort();
}

QString RamLoader::gotAssetGroup(QJsonObject newAG, RamProject *project)
{
    DBISuspender s;
    QString uuid = newAG.value("uuid").toString();

    // loop through existing asset groups to update them
    RamObjectUberList *projectAssetGroups = project->assetGroups();
    for (int i = projectAssetGroups->count() - 1; i >= 0; i--)
    {
        RamAssetGroup *existingAssetGroup = qobject_cast<RamAssetGroup*>( projectAssetGroups->at(i) );

        if (uuid == existingAssetGroup->uuid())
        {
            existingAssetGroup->setName( newAG.value("name").toString() );
            existingAssetGroup->setShortName( newAG.value("shortName").toString());

            gotAssets( newAG.value("assets").toArray(), existingAssetGroup, project);

            return uuid;
        }
    }

    // not existing, let's create it
    RamAssetGroup *assetGroup = new RamAssetGroup(
                newAG.value("shortName").toString(),
                newAG.value("name").toString(),
                newAG.value("projectUuid").toString(),
                newAG.value("uuid").toString()
                );

    //add assets
    gotAssets( newAG.value("assets").toArray(), assetGroup, project);

    projectAssetGroups->append(assetGroup);

    return uuid;
}

void RamLoader::gotAssets(QJsonArray assets, RamAssetGroup *assetGroup, RamProject *project)
{
    DBISuspender s;

    m_pm->addToMaximum(assets.count());
    m_pm->setText("Loading assets");

    QStringList uuids;

    // Update assets
    for (int j = 0; j < assets.count(); j++)
    {
        m_pm->increment();
        uuids << gotAsset( assets.at(j).toObject(), assetGroup, project );
    }

    // Remove deleted assets
    for (int i = assetGroup->count() - 1; i >= 0; i--)
    {
        RamObject *existingAsset = assetGroup->at(i);
        if (!uuids.contains(existingAsset->uuid()))
        {
            existingAsset->remove();
        }
    }

    // sort the asset group
    assetGroup->sort();
}

QString RamLoader::gotAsset(QJsonObject newA, RamAssetGroup *assetGroup, RamProject *project)
{
    DBISuspender s;
    QString uuid = newA.value("uuid").toString();

    // loop through existing shots to update them
    for (int i = assetGroup->count() - 1; i >= 0; i--)
    {
        RamAsset *existingAsset = qobject_cast<RamAsset*>( assetGroup->at(i) );

        if (uuid == existingAsset->uuid())
        {
            existingAsset->setName( newA.value("name").toString());
            existingAsset->setTags(newA.value("tags").toString());
            existingAsset->setShortName( newA.value("shortName").toString());

            gotStatusHistory( newA.value("statusHistory").toArray(), existingAsset, project);

            return uuid;
        }
    }

    // not existing, let's create it
    RamAsset *asset = new RamAsset(
                newA.value("shortName").toString(),
                newA.value("name").toString(),
                newA.value("assetGroupUuid").toString(),
                newA.value("uuid").toString()
                );
    asset->setTags( newA.value("tags").toString());
    gotStatusHistory( newA.value("statusHistory").toArray(), asset, project);

    assetGroup->append(asset);

    return uuid;
}

void RamLoader::gotSequences(QJsonArray sequences, RamProject *project)
{
    DBISuspender s;

    m_pm->addToMaximum(sequences.count());
    m_pm->setText("Loading sequences");
    qDebug() << "Loading sequences";

    QStringList uuids;
    // Update sequences
    for (int j = 0; j < sequences.count(); j++)
    {
        m_pm->increment();
        uuids << gotSequence( sequences.at(j).toObject(), project );
    }

    // Remove deleted sequences
    RamObjectUberList *projectSequences = project->sequences();
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

    // loop through existing sequences to update them
    RamObjectUberList *projectSequences = project->sequences();
    for (int i = projectSequences->count() - 1; i >= 0; i--)
    {
        RamSequence *existingSequence = qobject_cast<RamSequence*>( projectSequences->at(i) );

        if (uuid == existingSequence->uuid())
        {
            existingSequence->setName( newS.value("name").toString());
            existingSequence->setShortName( newS.value("shortName").toString());

            gotShots( newS.value("shots").toArray(), existingSequence, project);

            return uuid;
        }
    }

    // not existing, let's create it
    RamSequence *sequence = new RamSequence(
                newS.value("shortName").toString(),
                newS.value("name").toString(),
                newS.value("projectUuid").toString(),
                newS.value("uuid").toString()
                );

    //add shots
    gotShots( newS.value("shots").toArray(), sequence, project);

    projectSequences->append(sequence);

    return uuid;
}

void RamLoader::gotShots(QJsonArray shots, RamSequence *sequence, RamProject *project)
{
    DBISuspender s;

    m_pm->addToMaximum(shots.count());
    m_pm->setText("Loading shots...");

    QStringList uuids;
    // Update shots
    for (int j = 0; j < shots.count(); j++)
    {
        m_pm->increment();
        uuids << gotShot( shots.at(j).toObject(), sequence, project );
    }

    // Remove deleted shots
    for (int i = sequence->count() - 1; i >= 0; i--)
    {
        RamObject *existingShot = sequence->at(i);
        if (!uuids.contains(existingShot->uuid()))
        {
            existingShot->remove();
        }
    }

    // sort the sequence
    sequence->sort();
}

QString RamLoader::gotShot(QJsonObject newS, RamSequence *sequence, RamProject *project)
{
    DBISuspender s;
    QString uuid = newS.value("uuid").toString();

    // loop through existing shots to update them
    for (int i = sequence->count() - 1; i >= 0; i--)
    {
        RamShot *existingShot = qobject_cast<RamShot*>( sequence->at(i) );

        if (uuid == existingShot->uuid())
        {
            existingShot->setName( newS.value("name").toString());
            existingShot->setDuration( newS.value("duration").toDouble() );
            existingShot->setOrder( newS.value("order").toInt() );
            existingShot->setShortName( newS.value("shortName").toString());

            gotStatusHistory( newS.value("statusHistory").toArray(), existingShot, project);

            return uuid;
        }
    }

    // not existing, let's create it
    RamShot *shot = new RamShot(
                newS.value("shortName").toString(),
                newS.value("name").toString(),
                newS.value("sequenceUuid").toString(),
                newS.value("uuid").toString()
                );

    shot->setDuration( newS.value("duration").toDouble() );
    shot->setOrder( newS.value("order").toInt() );
    gotStatusHistory( newS.value("statusHistory").toArray(), shot, project);

    sequence->append(shot);

    return uuid;
}

void RamLoader::gotStatusHistory(QJsonArray statusHistory, RamItem *item, RamProject *project)
{
    DBISuspender s;

    QStringList uuids;
    // Update status
    for (int j = 0; j < statusHistory.count(); j++)
    {
        uuids << gotStatus( statusHistory.at(j).toObject(), item, project );
    }

    // Remove deleted status
    RamObjectUberList *history = item->statusHistory();
    for (int i = history->objectCount() - 1; i >= 0; i--)
    {
        RamStatus *existingStatus = qobject_cast<RamStatus*>( history->objectAt(i) );
        if (!uuids.contains(existingStatus->uuid()))
        {
            existingStatus->remove();
        }
    }

    // sort the status
    history->sort();
}

QString RamLoader::gotStatus(QJsonObject newS, RamItem *item, RamProject *project)
{
    DBISuspender s;
    QString uuid = newS.value("uuid").toString();

    // update existing if any
    RamObjectUberList *history = item->statusHistory();
    RamObject *o = history->objectFromUuid( uuid );
    if (o)
    {
        RamStatus *existingStatus = qobject_cast<RamStatus*>(o);
        existingStatus->setCompletionRatio( newS.value("completionRatio").toInt( ));

        // Don't set user: it can't (should not) change
        // Don't set step: it can't (should not) change

        // Get State
        RamObject *stateObj = m_ram->states()->fromUuid(  newS.value("stateUuid").toString( ) );
        RamState *state = qobject_cast<RamState*>( stateObj );
        if (state) existingStatus->setState( state );
        existingStatus->setComment( newS.value("comment").toString() );

        existingStatus->setDate( QDateTime::fromString( newS.value("date").toString(), "yyyy-MM-dd hh:mm:ss"));
        existingStatus->setVersion( newS.value("version").toInt() );

        return uuid;
    }

    // not existing, let's create it

    RamObject *user = m_ram->users()->fromUuid(  newS.value("userUuid").toString( ) );
    if (!user) return uuid;
    RamObject *stateObj = m_ram->states()->fromUuid(  newS.value("stateUuid").toString( ) );
    RamState *state = qobject_cast<RamState*>( stateObj );
    if (!state) return uuid;
    RamObject *stepObj = project->steps()->fromUuid( newS.value("stepUuid").toString( ) );
    RamStep *step = qobject_cast<RamStep*>( stepObj );
    if (!step) return uuid;
    RamStatus *status = new RamStatus(
                (RamUser*)user,
                state,
                step,
                item,
                newS.value("uuid").toString()
                );

    status->setCompletionRatio( newS.value("completionRatio").toInt( ) );
    status->setComment( newS.value("comment").toString( ) );
    status->setVersion( newS.value("version").toInt( ) );
    status->setDate( QDateTime::fromString( newS.value("date").toString(), "yyyy-MM-dd hh:mm:ss"));

    item->addStatus(status);

    return uuid;
}

void RamLoader::gotPipes(QJsonArray pipes, RamProject *project)
{
    DBISuspender s;

    m_pm->addToMaximum(pipes.count());
    m_pm->setText("Loading pipes...");
    qDebug() << "Loading pipes";

    QStringList uuids;
    // Update pipes
    for (int j = 0; j < pipes.count(); j++)
    {
        m_pm->increment();
        uuids << gotPipe( pipes.at(j).toObject(), project );
    }

    // Remove deleted pipes
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

    // loop through existing pipes to update them
    RamObjectList *projectPipeline = project->pipeline();
    for (int i = projectPipeline->count() - 1; i >= 0; i--)
    {
        RamPipe *existingPipe = qobject_cast<RamPipe*>( projectPipeline->at(i) );

        if (uuid == existingPipe->uuid())
        {
            RamObject *inputStepObj = project->steps()->fromUuid( newP.value("inputStepUuid").toString( ) );
            RamStep *inputStep = qobject_cast<RamStep*>( inputStepObj );
            if (inputStep) existingPipe->setInputStep( inputStep );
            RamObject *outputStepObj = project->steps()->fromUuid( newP.value("outputStepUuid").toString( ) );
            RamStep *outputStep = qobject_cast<RamStep*>( outputStepObj );
            if (outputStep) existingPipe->setOutputStep( outputStep );
            QJsonArray newPFs = newP.value("pipeFiles").toArray();
            for (int j = 0; j < newPFs.count(); j++)
            {
                RamPipeFile *pf = RamPipeFile::pipeFile( newPFs.at(j).toString() );
                if (pf) existingPipe->pipeFiles()->append(pf);
            }
            return uuid;
        }
    }

    // not existing, let's create it
    RamObject *inputStepObj = project->steps()->fromUuid( newP.value("inputStepUuid").toString( ) );
    RamStep *inputStep = qobject_cast<RamStep*>( inputStepObj );
    RamObject *outputStepObj = project->steps()->fromUuid( newP.value("outputStepUuid").toString( ) );
    RamStep *outputStep = qobject_cast<RamStep*>( outputStepObj );
    if (inputStep && outputStep)
    {
        RamPipe *pipe = new RamPipe(
                    outputStep,
                    inputStep,
                    newP.value("uuid").toString()
                    );

        QJsonArray newPFs = newP.value("pipeFiles").toArray();
        for (int j = 0; j < newPFs.count(); j++)
        {
            RamPipeFile *pf = RamPipeFile::pipeFile( newPFs.at(j).toString() );
            if (pf) pipe->pipeFiles()->append(pf);
        }

        projectPipeline->append( pipe );
    }

    return uuid;
}

void RamLoader::gotPipeFiles(QJsonArray pipeFiles, RamProject *project)
{
    DBISuspender s;

    m_pm->addToMaximum(pipeFiles.count());
    m_pm->setText("Loading pipe files...");
    qDebug() << "Loading pipe files";

    QStringList uuids;
    // Update pipes
    for (int j = 0; j < pipeFiles.count(); j++)
    {
        m_pm->increment();
        uuids << gotPipeFile( pipeFiles.at(j).toObject(), project );
    }

    // Remove deleted pipes
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

    pipeFile->setProjectUuid(newPF.value("projectUuid").toString());

    RamFileType *ft = RamFileType::fileType( newPF.value("fileTypeUuid").toString() );
    if (ft) pipeFile->setFileType(ft);

    project->pipeFiles()->append( pipeFile );

    return uuid;
}
