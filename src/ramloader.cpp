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
}

void RamLoader::run()
{
    m_pm->start();
    m_pm->setTitle("Loading new data.");

    QString query = m_data.value("query").toString();

    if (query == "login") login( m_data.value("content").toObject() );
    else if (query == "getUsers") gotUsers( m_data.value("content").toArray());
    else if (query == "getProjects") gotProjects( m_data.value("content").toArray());
    else if (query == "getProject") {
        m_pm->setTitle("Loading project.");
        gotProject( m_data.value("content").toObject());
        //TODO emit currentProjectChanged(_currentProject);
    }
    else if (query == "getTemplateSteps") gotTemplateSteps( m_data.value("content").toArray());
    else if (query == "getTemplateAssetGroups") gotTemplateAssetGroups( m_data.value("content").toArray());
    else if (query == "getStates") gotStates( m_data.value("content").toArray());
    else if (query == "getFileTypes") gotFileTypes( m_data.value("content").toArray());
    else if (query == "getApplications") gotApplications( m_data.value("content").toArray());
    else if (query == "init")
    {
        m_pm->setTitle("Getting Ramses data.");
        QJsonObject content = m_data.value("content").toObject();
        gotUsers( content.value("users").toArray());
        gotTemplateSteps( content.value("templateSteps").toArray());
        gotTemplateAssetGroups( content.value("templateAssetGroups").toArray());
        gotStates( content.value("states").toArray());
        gotFileTypes( content.value("fileTypes").toArray());
        gotApplications( content.value("applications").toArray());
        gotProjects( content.value("projects").toArray());
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
                //Emit just one signal
                QSignalBlocker b(existingUser);
                existingUser->setName( newUser.value("name").toString());
                existingUser->setShortName( newUser.value("shortName").toString());
                existingUser->setFolderPath( newUser.value("folderPath").toString());
                //send the signal
                b.unblock();
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

    // Set the current user
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

void RamLoader::gotProjects(QJsonArray projects)
{
    DBISuspender s;

    m_pm->addToMaximum(projects.count());
    m_pm->setText("Loading projects...");

    QStringList uuids;
    // Update projects
    for (int j = 0; j < projects.count(); j++)
    {
        m_pm->increment();
        uuids << gotProject( projects.at(j).toObject() );
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

QString RamLoader::gotProject(QJsonObject newP)
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
            //Emit just one signal
            QSignalBlocker b(existingProject);
            existingProject->setName( newP.value("name").toString());
            existingProject->setShortName( newP.value("shortName").toString());
            existingProject->setWidth( newP.value("width").toInt());
            existingProject->setHeight( newP.value("height").toInt());
            existingProject->setFramerate( newP.value("framerate").toDouble());
            gotSteps( newP.value("steps").toArray(), existingProject);
            gotAssetGroups( newP.value("assetGroups").toArray(), existingProject);
            gotSequences( newP.value("sequences").toArray(), existingProject);
            gotPipes( newP.value("pipes").toArray(), existingProject);
            //send the signal
            b.unblock();
            existingProject->setFolderPath( newP.value("folderPath").toString());
            return uuid;
        }
    }

    // not existing, let's create it
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
    gotSteps( newP.value("steps").toArray(), project);
    gotAssetGroups( newP.value("assetGroups").toArray(), project);
    gotSequences( newP.value("sequences").toArray(), project);
    gotPipes( newP.value("pipes").toArray(), project);

    _projects->append(project);

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
                //Emit just one signal
                QSignalBlocker b(existingStep);
                existingStep->setName( newStep.value("name").toString());
                existingStep->setShortName( newStep.value("shortName").toString());
                //send the signal
                b.unblock();
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
                //Emit just one signal
                QSignalBlocker b(existingAssetGroup);
                existingAssetGroup->setName( newAssetGroup.value("name").toString());
                //send the signal
                b.unblock();
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
                //Emit just one signal
                QSignalBlocker b(existingState);
                existingState->setName( newState.value("name").toString());
                existingState->setShortName( newState.value("shortName").toString());
                existingState->setColor( QColor(newState.value("color").toString()));
                //send the signal
                b.unblock();
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
                //Emit just one signal
                QSignalBlocker b(existingFileType);
                existingFileType->setName( newFileType.value("name").toString());
                existingFileType->setShortName( newFileType.value("shortName").toString() );
                existingFileType->setPreviewable( newFileType.value("previewable").toInt() != 0 );
                //send the signal
                b.unblock();
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

    // loop through existing steps to update them
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
                //Emit just one signal
                QSignalBlocker b(existingApplication);
                existingApplication->setName( newApplication.value("name").toString());
                existingApplication->setShortName( newApplication.value("shortName").toString());
                existingApplication->exportFileTypes()->clear();
                existingApplication->importFileTypes()->clear();
                existingApplication->nativeFileTypes()->clear();
                foreach( QJsonValue ft, newApplication.value("fileTypes").toArray())
                {
                    QJsonObject fileT = ft.toObject();
                    if (fileT.value("type").toString() == "import" )
                        existingApplication->importFileTypes()->append( m_ram->fileType( fileT.value("uuid").toString() ) );
                    else if (fileT.value("type").toString() == "export" )
                        existingApplication->exportFileTypes()->append( m_ram->fileType( fileT.value("uuid").toString() ) );
                    else
                        existingApplication->nativeFileTypes()->append( m_ram->fileType( fileT.value("uuid").toString() ) );
                }
                //send the signal
                b.unblock();
                existingApplication->setExecutableFilePath(newApplication.value("executableFilePath").toString());
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

    // loop through remaining new projects to add them
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
            if (fileT.value("type").toString() == "import" )
                app->importFileTypes()->append( m_ram->fileType( fileT.value("uuid").toString() ) );
            else if (fileT.value("type").toString() == "export" )
                app->exportFileTypes()->append( m_ram->fileType( fileT.value("uuid").toString() ) );
            else
                app->nativeFileTypes()->append( m_ram->fileType( fileT.value("uuid").toString() ) );
        }

        _applications->append(app);
    }
}

void RamLoader::gotSteps(QJsonArray steps, RamProject *project)
{
    DBISuspender s;

    m_pm->addToMaximum(steps.count());
    m_pm->setText("Loading steps...");

    QList<RamStep*> projectSteps = project->steps();

    // loop through existing steps to update them
    for (int i = projectSteps.count() - 1; i >= 0; i--)
    {
        m_pm->increment();
        RamStep *existingStep = projectSteps[i];
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
                //Emit just one signal
                QSignalBlocker b(existingStep);
                existingStep->setName( newStep.value("name").toString());
                existingStep->setShortName( newStep.value("shortName").toString());
                existingStep->setOrder( newStep.value("order").toInt());
                existingStep->clearUsers();

                foreach( QJsonValue u, newStep.value("users").toArray())
                    existingStep->assignUser( m_ram->users()->fromUuid( u.toString() ) );

                foreach(QJsonValue a, newStep.value("applications").toArray())
                    existingStep->assignApplication( m_ram->application(a.toString()));

                //send the signal
                b.unblock();
                existingStep->setType(newStep.value("type").toString());

                //remove from new steps
                steps.removeAt(j);
                break;
            }
        }
        // Not found, remove from existing
        if (!found)
        {
            project->removeStep(existingStep);
        }
    }

    // loop through remaining new steps to add them
    for (int i = 0; i < steps.count(); i++)
    {
        m_pm->increment();
        QJsonObject s = steps[i].toObject();
        RamStep *step = new RamStep(
                    s.value("shortName").toString(),
                    s.value("name").toString(),
                    false,
                    s.value("uuid").toString()
                    );
        step->setType( s.value("type").toString());
        step->setOrder( s.value("order").toInt() );
        step->setProjectUuid( s.value("projectUuid").toString());

        foreach( QJsonValue u, s.value("users").toArray())
            step->assignUser( m_ram->users()->fromUuid(u.toString()) );

        foreach(QJsonValue a, s.value("applications").toArray())
            step->assignApplication( m_ram->application(a.toString()));

        project->addStep(step);

        //emit newStep(step);
    }

    // sort the steps
    project->sortSteps();
}

void RamLoader::gotAssetGroups(QJsonArray assetGroups, RamProject *project)
{
    DBISuspender s;

    m_pm->addToMaximum(assetGroups.count());
    m_pm->setText("Loading asset groups...");

    QList<RamAssetGroup*> projectAssetGroups = project->assetGroups();

    // loop through existing asset groups to update them
    for (int i = projectAssetGroups.count() - 1; i >= 0; i--)
    {
        m_pm->increment();
        RamAssetGroup *existingAssetGroup = projectAssetGroups[i];
        // loop through new steps to update
        bool found = false;
        for (int j = assetGroups.count() - 1; j >= 0; j--)
        {
            QJsonObject newAG = assetGroups[j].toObject();
            QString uuid = newAG.value("uuid").toString();
            // Found, update
            if (uuid == existingAssetGroup->uuid())
            {
                found = true;
                //Emit just one signal
                QSignalBlocker b(existingAssetGroup);
                existingAssetGroup->setName( newAG.value("name").toString());
                gotAssets( newAG.value("assets").toArray(), existingAssetGroup, project);
                b.unblock();
                existingAssetGroup->setShortName( newAG.value("shortName").toString());
                //remove from new asset groups
                assetGroups.removeAt(j);
                break;
            }
        }
        // Not found, remove from existing
        if (!found)
        {
            project->removeAssetGroup(existingAssetGroup);
        }
    }

    // loop through remaining new asset groups to add them
    for (int i = 0; i < assetGroups.count(); i++)
    {
        m_pm->increment();
        QJsonObject ag = assetGroups[i].toObject();
        RamAssetGroup *assetGroup = new RamAssetGroup(
                    ag.value("shortName").toString(),
                    ag.value("name").toString(),
                    false,
                    ag.value("uuid").toString()
                    );
        assetGroup->setProjectUuid( ag.value("projectUuid").toString());

        //add assets
        gotAssets( ag.value("assets").toArray(), assetGroup, project);
        project->addAssetGroup(assetGroup);
    }

    // sort the asset groups
    project->sortAssetGroups();
}

void RamLoader::gotAssets(QJsonArray assets, RamAssetGroup *assetGroup, RamProject *project)
{
    DBISuspender s;

    m_pm->addToMaximum(assets.count());
    m_pm->setText("Loading assets");

    QList<RamAsset*> groupAssets = assetGroup->assets();

    // loop through existing assets to update them
    for (int i = groupAssets.count() - 1; i >= 0; i--)
    {
        m_pm->increment();
        RamAsset *existingAsset = groupAssets[i];
        // loop through new steps to update
        bool found = false;
        for (int j = assets.count() - 1; j >= 0; j--)
        {
            QJsonObject newA = assets[j].toObject();
            QString uuid = newA.value("uuid").toString();
            // Found, update
            if (uuid == existingAsset->uuid())
            {
                found = true;
                //Emit just one signal
                QSignalBlocker b(existingAsset);
                existingAsset->setName( newA.value("name").toString());
                existingAsset->setTags(newA.value("tags").toString());
                gotStatusHistory( newA.value("statusHistory").toArray(), existingAsset, project);
                b.unblock();
                existingAsset->setShortName( newA.value("shortName").toString());
                //remove from new asset groups
                assets.removeAt(j);
                break;
            }
        }
        // Not found, remove from existing
        if (!found)
        {
            assetGroup->removeAsset(existingAsset);
        }
    }

    // loop through remaining new assets to add them
    for (int i = 0; i < assets.count(); i++)
    {
        m_pm->increment();
        QJsonObject a = assets[i].toObject();
        RamAsset *asset = new RamAsset(
                    a.value("shortName").toString(),
                    a.value("name").toString(),
                    a.value("assetGroupUuid").toString(),
                    a.value("uuid").toString()
                    );
        asset->setTags( a.value("tags").toString());
        gotStatusHistory( a.value("statusHistory").toArray(), asset, project);

        assetGroup->addAsset(asset);
    }

    // sort the asset groups
    assetGroup->sortAssets();
}

void RamLoader::gotSequences(QJsonArray sequences, RamProject *project)
{
    DBISuspender s;

    m_pm->addToMaximum(sequences.count());
    m_pm->setText("Loading sequences");

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
        RamSequence *existingSequence = qobject_cast<RamSequence*>( projectSequences->at(i) );
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
            //Emit just one signal
            QSignalBlocker b(existingSequence);
            existingSequence->setName( newS.value("name").toString());
            //add shots
            gotShots( newS.value("shots").toArray(), existingSequence, project);
            b.unblock();
            existingSequence->setShortName( newS.value("shortName").toString());
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
            sequence->removeAll(existingShot->uuid());
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
        RamShot *existingShot = (RamShot*)sequence->at(i);

        if (uuid == existingShot->uuid())
        {
            //Emit just one signal
            QSignalBlocker b(existingShot);
            existingShot->setName( newS.value("name").toString());
            existingShot->setDuration( newS.value("duration").toDouble() );
            existingShot->setOrder( newS.value("order").toInt() );
            gotStatusHistory( newS.value("statusHistory").toArray(), existingShot, project);
            b.unblock();
            existingShot->setShortName( newS.value("shortName").toString());
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
            history->removeObject(existingStatus);
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
        //Emit just one signal
        QSignalBlocker b(existingStatus);
        existingStatus->setCompletionRatio( newS.value("completionRatio").toInt( ));
        RamObject *u = m_ram->users()->fromUuid(  newS.value("userUuid").toString( ) );
        if (u) existingStatus->setUser( u );
        RamState *state = (RamState*)m_ram->states()->fromUuid(  newS.value("stateUuid").toString( ) );
        if (state) existingStatus->setState( state );
        existingStatus->setComment( newS.value("comment").toString() );
        RamStep *step = project->step( newS.value("stepUuid").toString( ) );
        if (step) existingStatus->setStep( step );
        existingStatus->setDate( QDateTime::fromString( newS.value("date").toString(), "yyyy-MM-dd hh:mm:ss"));
        b.unblock();
        existingStatus->setVersion( newS.value("version").toInt() );
        return uuid;
    }

    // not existing, let's create it

    RamObject *user = m_ram->users()->fromUuid(  newS.value("userUuid").toString( ) );
    if (!user) return uuid;
    RamState *state = (RamState*)m_ram->states()->fromUuid( newS.value("stateUuid").toString( ) );
    if (!state) return uuid;
    RamStep *step = project->step( newS.value("stepUuid").toString( ) );
    if (!step) return uuid;
    RamStatus *status = new RamStatus(
                (RamUser*)user,
                state,
                step,
                newS.value("uuid").toString(),
                item
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

    QStringList uuids;
    // Update pipes
    for (int j = 0; j < pipes.count(); j++)
    {
        m_pm->increment();
        uuids << gotPipe( pipes.at(j).toObject(), project );
    }

    // Remove deleted pipes
    QList<RamPipe*> projectPipeline = project->pipeline();
    for (int i = projectPipeline.count() - 1; i >= 0; i--)
    {
        RamPipe *existingPipe = projectPipeline.at(i);
        if (!uuids.contains(existingPipe->uuid()))
        {
            project->removePipe(existingPipe->uuid());
        }
    }
}

QString RamLoader::gotPipe(QJsonObject newP, RamProject *project)
{
    DBISuspender s;
    QString uuid = newP.value("uuid").toString();

    // loop through existing pipes to update them
    QList<RamPipe*> projectPipeline = project->pipeline();
    for (int i = projectPipeline.count() - 1; i >= 0; i--)
    {
        RamPipe *existingPipe = projectPipeline.at(i);

        if (uuid == existingPipe->uuid())
        {
            //Emit just one signal
            QSignalBlocker b(existingPipe);
            RamStep *inputStep =  project->step(newP.value("inputStepUuid").toString());
            if (inputStep) existingPipe->setInputStep( inputStep );
            RamStep *outputStep =  project->step(newP.value("outputStepUuid").toString());
            if (outputStep) existingPipe->setOutputStep( outputStep );
            b.unblock();
            RamFileType *ft = m_ram->fileType( newP.value("filetypeUuid").toString()) ;
            existingPipe->setFileType( ft );
            return uuid;
        }
    }

    // not existing, let's create it
    RamStep *inputStep =  project->step(newP.value("inputStepUuid").toString());
    RamStep *outputStep =  project->step(newP.value("outputStepUuid").toString());
    if (inputStep && outputStep)
    {
        RamPipe *pipe = new RamPipe(
                    outputStep,
                    inputStep,
                    newP.value("uuid").toString()
                    );

        RamFileType *ft = m_ram->fileType( newP.value("filetypeUuid").toString()) ;
        if (ft) pipe->setFileType( ft );

        project->addPipe(pipe);
    }

    return uuid;
}
