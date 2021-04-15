#include "ramses.h"

Ramses *Ramses::_instance = nullptr;

Ramses *Ramses::instance()
{
    if(!_instance) _instance = new Ramses();
    return _instance;
}

Ramses::~Ramses()
{
    DBInterface::instance()->suspend(true);
}

Ramses::Ramses(QObject *parent) : QObject(parent)
{
    _dbi = DBInterface::instance();

    DBISuspender s;

    _currentUser = nullptr;
    _currentProject = nullptr;
    _ramsesPath = _settings.value("ramsesPath", QDir::homePath() + "/Ramses").toString();

    _connected = false;

    _userSettings = new QSettings(QSettings::IniFormat, QSettings::UserScope, STR_COMPANYNAME, STR_INTERNALNAME);
    _userSettings->beginGroup("ramses");

    connect( _dbi, &DBInterface::data, this, &Ramses::newData );
    connect( _dbi, &DBInterface::connectionStatusChanged, this, &Ramses::dbiConnectionStatusChanged);
    connect( (DuApplication *)qApp, &DuApplication::idle, this, &Ramses::refresh);
}

void Ramses::login(QString username, QString password)
{
    _dbi->login(username, password);
}

void Ramses::newData(QJsonObject data)
{
    if (!data.value("success").toBool()) return;

    QString query = data.value("query").toString();
    if (query == "login") login( data.value("content").toObject() );
    else if (query == "getUsers") gotUsers( data.value("content").toArray());
    else if (query == "getProjects") gotProjects( data.value("content").toArray());
    else if (query == "getProject") gotProject( data.value("content").toObject());
    else if (query == "getTemplateSteps") gotTemplateSteps( data.value("content").toArray());
    else if (query == "getTemplateAssetGroups") gotTemplateAssetGroups( data.value("content").toArray());
    else if (query == "getStates") gotStates( data.value("content").toArray());
    else if (query == "getFileTypes") gotFileTypes( data.value("content").toArray());
    else if (query == "getApplications") gotApplications( data.value("content").toArray());
}

void Ramses::gotUsers(QJsonArray users)
{
    DBISuspender s;

    // loop through existing users to update them
    for (int i = _users.count() - 1; i >= 0; i--)
    {
        RamUser *existingUser = _users[i];
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
            RamUser *u = _users.takeAt(i);
            u->remove();
        }
    }

    // loop through remaining new users to add them
    for (int i = 0; i < users.count(); i++)
    {
        QJsonObject u = users[i].toObject();
        RamUser *user = new RamUser(
                    u.value("shortName").toString(),
                    u.value("name").toString(),
                    u.value("uuid").toString()
                    );
        user->setFolderPath(u.value("folderPath").toString());
        user->setRole( u.value("role").toString("standard") );

        _users << user;

        connect(user,&RamUser::removed, this, &Ramses::userRemoved);
        emit newUser(user);
    }

    // Set the current user
    foreach(RamUser *user, _users)
    {
        if (user->shortName() == _currentUserShortName)
        {
            _currentUser = user;
            _connected = true;

            emit loggedIn(_currentUser);

            return;
        }
    }

    // Not found
    _currentUser = nullptr;
    _connected = false;
    setCurrentProject(nullptr);
    emit loggedOut();
}

void Ramses::userRemoved(RamObject *o)
{
    removeUser(o->uuid());
}

void Ramses::gotProjects(QJsonArray projects)
{
    DBISuspender s;

    QStringList uuids;
    // Update projects
    for (int j = 0; j < projects.count(); j++)
    {
        uuids << gotProject( projects[j].toObject() );
    }

    // Remove deleted projects
    for (int i = _projects.count() - 1; i >= 0; i--)
    {
        RamProject *existingProject = _projects[i];
        if (!uuids.contains(existingProject->uuid()))
        {
            RamProject *p = _projects.takeAt(i);
            p->remove();
        }
    }

    // Set settings
    _userSettings->endGroup();
    delete _userSettings;
    _userSettings = new QSettings(Ramses::instance()->currentUserSettingsFile(), QSettings::IniFormat, this);
    _userSettings->beginGroup("ramses");

    setCurrentProject(_userSettings->value("currentProject", "").toString());
}

QString Ramses::gotProject(QJsonObject newP)
{
    DBISuspender s;
    QString uuid = newP.value("uuid").toString();

    // loop through existing projects to update them
    for (int i = _projects.count() - 1; i >= 0; i--)
    {
        RamProject *existingProject = _projects[i];

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

    _projects << project;

    connect(project,&RamProject::removed, this, &Ramses::projectRemoved);

    emit newProject(project);

    return uuid;
}

void Ramses::projectRemoved(RamObject *o)
{
    removeProject(o->uuid());
}

void Ramses::gotTemplateSteps(QJsonArray steps)
{
    DBISuspender s;

    // loop through existing steps to update them
    for (int i = _templateSteps.count() - 1; i >= 0; i--)
    {
        RamStep *existingStep = _templateSteps[i];
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
            RamStep *s = _templateSteps.takeAt(i);
            s->remove();
        }
    }

    // loop through remaining new projects to add them
    for (int i = 0; i < steps.count(); i++)
    {
        QJsonObject s = steps[i].toObject();
        RamStep *step = new RamStep(
                    s.value("shortName").toString(),
                    s.value("name").toString(),
                    true,
                    s.value("uuid").toString()
                    );
        step->setType( s.value("type").toString());

        _templateSteps << step;

        connect(step,&RamStep::removed, this, &Ramses::templateStepRemoved);

        emit newTemplateStep(step);
    }
}

void Ramses::templateStepRemoved(RamObject *o)
{
    removeTemplateStep(o->uuid());
}

void Ramses::gotTemplateAssetGroups(QJsonArray assetGroups)
{
    DBISuspender s;

    // loop through existing asset groups to update them
    for (int i = _templateAssetGroups.count() - 1; i >= 0; i--)
    {
        RamAssetGroup *existingAssetGroup = _templateAssetGroups[i];
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
            RamAssetGroup *ag = _templateAssetGroups.takeAt(i);
            ag->remove();
        }
    }

    // loop through remaining new projects to add them
    for (int i = 0; i < assetGroups.count(); i++)
    {
        QJsonObject ag = assetGroups[i].toObject();
        RamAssetGroup *assetGroup = new RamAssetGroup(
                    ag.value("shortName").toString(),
                    ag.value("name").toString(),
                    true,
                    ag.value("uuid").toString()
                    );

        _templateAssetGroups << assetGroup;

        connect(assetGroup,&RamAssetGroup::removed, this, &Ramses::templateAssetGroupRemoved);

        emit newTemplateAssetGroup(assetGroup);
    }
}

void Ramses::templateAssetGroupRemoved(RamObject *o)
{
    removeTemplateAssetGroup(o->uuid());
}

void Ramses::gotStates(QJsonArray states)
{
    DBISuspender s;

    // loop through existing steps to update them
    for (int i = _states.count() - 1; i >= 0; i--)
    {
        RamState *existingState = _states[i];
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
            RamState *s = _states.takeAt(i);
            s->remove();
        }
    }

    // loop through remaining new projects to add them
    for (int i = 0; i < states.count(); i++)
    {
        QJsonObject s = states[i].toObject();
        RamState *state = new RamState(
                    s.value("shortName").toString(),
                    s.value("name").toString(),
                    s.value("uuid").toString()
                    );
        state->setColor( QColor( s.value("color").toString()) );
        state->setCompletionRatio( s.value("completionRatio").toInt() );

        _states << state;

        connect(state,&RamState::removed, this, &Ramses::stateRemoved);

        emit newState(state);
    }
}

void Ramses::stateRemoved(RamObject *o)
{
    removeState(o->uuid());
}

void Ramses::gotFileTypes(QJsonArray fileTypes)
{
    DBISuspender s;

    // loop through existing file types to update them
    for (int i = _fileTypes.count() - 1; i >= 0; i--)
    {
        RamFileType *existingFileType = _fileTypes[i];
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
            RamFileType *ft = _fileTypes.takeAt(i);
            ft->remove();
        }
    }

    // loop through remaining new file types to add them
    for (int i = 0; i < fileTypes.count(); i++)
    {
        QJsonObject ft = fileTypes[i].toObject();
        RamFileType *fileType = new RamFileType(
                    ft.value("shortName").toString(),
                    ft.value("name").toString(),
                    ft.value("extensions").toString(),
                    ft.value("uuid").toString()
                    );
        fileType->setPreviewable( ft.value("previewable").toInt() != 0 );

        _fileTypes << fileType;

        connect(fileType,SIGNAL(removed(RamObject*)), this, SLOT(removeFileType(RamObject*)));

        emit newFileType(fileType);
    }
}

void Ramses::gotApplications(QJsonArray applications)
{
    DBISuspender s;

    // loop through existing steps to update them
    for (int i = _applications.count() - 1; i >= 0; i--)
    {
        RamApplication *existingApplication = _applications[i];
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
                existingApplication->clearExportFileTypes();
                existingApplication->clearImportFileTypes();
                existingApplication->clearNativeFileTypes();
                foreach( QJsonValue ft, newApplication.value("fileTypes").toArray())
                {
                    QJsonObject fileT = ft.toObject();
                    if (fileT.value("type").toString() == "import" )
                        existingApplication->assignImportFileType( fileType( fileT.value("uuid").toString() ) );
                    else if (fileT.value("type").toString() == "export" )
                        existingApplication->assignExportFileType( fileType( fileT.value("uuid").toString() ) );
                    else
                        existingApplication->assignNativeFileType( fileType( fileT.value("uuid").toString() ) );
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
            RamApplication *a = _applications.takeAt(i);
            a->remove();
        }
    }

    // loop through remaining new projects to add them
    for (int i = 0; i < applications.count(); i++)
    {
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
                app->assignImportFileType( fileType( fileT.value("uuid").toString() ) );
            else if (fileT.value("type").toString() == "export" )
                app->assignExportFileType( fileType( fileT.value("uuid").toString() ) );
            else
                app->assignNativeFileType( fileType( fileT.value("uuid").toString() ) );
        }

        _applications << app;

        connect(app,SIGNAL(removed(RamObject*)), this, SLOT(removeApplication(RamObject*)));

        emit newApplication(app);
    }
}

void Ramses::gotSteps(QJsonArray steps, RamProject *project)
{
    DBISuspender s;

    QList<RamStep*> projectSteps = project->steps();

    // loop through existing steps to update them
    for (int i = projectSteps.count() - 1; i >= 0; i--)
    {
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
                    existingStep->assignUser( user(u.toString()) );

                foreach(QJsonValue a, newStep.value("applications").toArray())
                    existingStep->assignApplication( application(a.toString()));

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
            step->assignUser( user(u.toString()) );

        foreach(QJsonValue a, s.value("applications").toArray())
            step->assignApplication( application(a.toString()));

        project->addStep(step);

        emit newStep(step);
    }

    // sort the steps
    project->sortSteps();
}

void Ramses::gotAssetGroups(QJsonArray assetGroups, RamProject *project)
{
    DBISuspender s;

    QList<RamAssetGroup*> projectAssetGroups = project->assetGroups();

    // loop through existing asset groups to update them
    for (int i = projectAssetGroups.count() - 1; i >= 0; i--)
    {
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
                gotAssets( newAG.value("assets").toArray(), existingAssetGroup);
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
        QJsonObject ag = assetGroups[i].toObject();
        RamAssetGroup *assetGroup = new RamAssetGroup(
                    ag.value("shortName").toString(),
                    ag.value("name").toString(),
                    false,
                    ag.value("uuid").toString()
                    );
        assetGroup->setProjectUuid( ag.value("projectUuid").toString());

        //add assets
        gotAssets( ag.value("assets").toArray(), assetGroup);

        project->addAssetGroup(assetGroup);
    }

    // sort the asset groups
    project->sortAssetGroups();
}

void Ramses::gotAssets(QJsonArray assets, RamAssetGroup *assetGroup)
{
    DBISuspender s;

    QList<RamAsset*> groupAssets = assetGroup->assets();

    // loop through existing assets to update them
    for (int i = groupAssets.count() - 1; i >= 0; i--)
    {
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
        QJsonObject a = assets[i].toObject();
        RamAsset *asset = new RamAsset(
                    a.value("shortName").toString(),
                    a.value("name").toString(),
                    a.value("assetGroupUuid").toString(),
                    a.value("uuid").toString()
                    );
        asset->setTags( a.value("tags").toString());

        assetGroup->addAsset(asset);
    }

    // sort the asset groups
    assetGroup->sortAssets();
}

void Ramses::gotSequences(QJsonArray sequences, RamProject *project)
{
    DBISuspender s;
    QStringList uuids;
    // Update sequences
    for (int j = 0; j < sequences.count(); j++)
    {
        uuids << gotSequence( sequences.at(j).toObject(), project );
    }

    // Remove deleted sequences
    QList<RamSequence*> projectSequences = project->sequences();
    for (int i = projectSequences.count() - 1; i >= 0; i--)
    {
        RamSequence *existingSequence = projectSequences.at(i);
        if (!uuids.contains(existingSequence->uuid()))
        {
            project->removeSequence(existingSequence->uuid());
        }
    }

    // sort the sequences
    project->sortSequences();
}

QString Ramses::gotSequence(QJsonObject newS, RamProject *project)
{
    DBISuspender s;
    QString uuid = newS.value("uuid").toString();

    // loop through existing sequences to update them
    QList<RamSequence*> projectSequences = project->sequences();
    for (int i = projectSequences.count() - 1; i >= 0; i--)
    {
        RamSequence *existingSequence = projectSequences.at(i);

        if (uuid == existingSequence->uuid())
        {
            //Emit just one signal
            QSignalBlocker b(existingSequence);
            existingSequence->setName( newS.value("name").toString());
            //add shots
            gotShots( newS.value("shots").toArray(), existingSequence);
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
    gotShots( newS.value("shots").toArray(), sequence);

    project->addSequence(sequence);

    return uuid;
}

void Ramses::gotShots(QJsonArray shots, RamSequence *sequence)
{
    DBISuspender s;
    QStringList uuids;
    // Update shots
    for (int j = 0; j < shots.count(); j++)
    {
        uuids << gotShot( shots.at(j).toObject(), sequence );
    }

    // Remove deleted shots
    QList<RamShot*> sequenceShots = sequence->shots();
    for (int i = sequenceShots.count() - 1; i >= 0; i--)
    {
        RamShot *existingShot = sequenceShots.at(i);
        if (!uuids.contains(existingShot->uuid()))
        {
            sequence->removeShot(existingShot->uuid());
        }
    }

    // sort the sequences
    sequence->sortShots();
}

QString Ramses::gotShot(QJsonObject newS, RamSequence *sequence)
{
    DBISuspender s;
    QString uuid = newS.value("uuid").toString();

    // loop through existing shots to update them
    QList<RamShot*> sequenceShots = sequence->shots();
    for (int i = sequenceShots.count() - 1; i >= 0; i--)
    {
        RamShot *existingShot = sequenceShots.at(i);

        if (uuid == existingShot->uuid())
        {
            //Emit just one signal
            QSignalBlocker b(existingShot);
            existingShot->setName( newS.value("name").toString());
            existingShot->setDuration( newS.value("duration").toDouble() );
            existingShot->setOrder( newS.value("order").toInt() );
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

    sequence->addShot(shot);

    return uuid;
}

void Ramses::gotPipes(QJsonArray pipes, RamProject *project)
{
    DBISuspender s;

    QList<RamPipe*> projectPipeline = project->pipeline();

    // loop through existing pipeline to update pipes
    for (int i = projectPipeline.count() - 1; i >= 0; i--)
    {
        RamPipe *existingPipe = projectPipeline[i];
        // loop through new pipes to update
        bool found = false;
        for (int j = pipes.count() - 1; j >= 0; j--)
        {
            QJsonObject newP = pipes[j].toObject();
            QString uuid = newP.value("uuid").toString();
            // Found, update
            if (uuid == existingPipe->uuid())
            {
                found = true;
                //Emit just one signal
                QSignalBlocker b(existingPipe);
                RamStep *inputStep =  project->step(newP.value("inputStepUuid").toString());
                if (inputStep) existingPipe->setInputStep( inputStep );
                RamStep *outputStep =  project->step(newP.value("outputStepUuid").toString());
                if (outputStep) existingPipe->setOutputStep( outputStep );
                b.unblock();
                RamFileType *ft = fileType( newP.value("filetypeUuid").toString()) ;
                existingPipe->setFileType( ft );
                //remove from new pipes
                pipes.removeAt(j);
                break;
            }
        }
        // Not found, remove from existing
        if (!found)
        {
            project->removePipe(existingPipe);
        }
    }

    // loop through remaining new asset groups to add them
    for (int i = 0; i < pipes.count(); i++)
    {
        QJsonObject p = pipes[i].toObject();
        RamStep *inputStep =  project->step(p.value("inputStepUuid").toString());
        RamStep *outputStep =  project->step(p.value("outputStepUuid").toString());
        if (inputStep && outputStep)
        {
            RamPipe *pipe = new RamPipe(
                        outputStep,
                        inputStep,
                        p.value("uuid").toString()
                        );

            RamFileType *ft = fileType( p.value("filetypeUuid").toString()) ;
            if (ft) pipe->setFileType( ft );

            project->addPipe(pipe);
        }
    }
}

void Ramses::dbiConnectionStatusChanged(NetworkUtils::NetworkStatus s)
{
    if (s != NetworkUtils::Online)
    {
        QSignalBlocker b(_dbi);
        logout();
    }
}

void Ramses::login(QJsonObject user)
{
    // Set the current user shortName
    _currentUserShortName = user.value("shortName").toString("Guest");
    // Update
    refresh();
}

QDir Ramses::createPath(QString p) const
{
    QDir d(p);
    if (p != "") d.mkpath(".");
    return d;
}

QString Ramses::ramsesPath() const
{
    return _ramsesPath;
}

void Ramses::setRamsesPath(const QString &ramsesPath)
{
    _ramsesPath = ramsesPath;
}

QString Ramses::pathFromRamses(QString p) const
{
    if (QFileInfo( p ).isRelative())
    {
        return ramsesPath() + "/" +p;
    }
    else
    {
        return p;
    }
}

QString Ramses::usersPath() const
{
    return ramsesPath() + "/Users";
}

QString Ramses::path(RamUser *u) const
{
    if (!u) return pathFromRamses("");

    QString path = u->folderPath();
    if (path == "" || path == "auto") path = usersPath() + "/" + u->shortName();
    return pathFromRamses( path );
}

QDir Ramses::dir(RamUser *u) const
{
    return createPath(path(u));
}

QString Ramses::currentUserPath() const
{
    RamUser *u = this->currentUser();
    if (!u) return "";
    else return path(u);
}

QDir Ramses::currentUserDir() const
{
    return createPath(currentUserPath());
}

QString Ramses::configPath(RamUser *u) const
{
    return path(u) + "/Config";
}

QDir Ramses::configDir(RamUser *u) const
{
    return createPath(configPath(u));
}

QString Ramses::currentUserConfigPath() const
{
    RamUser *u = this->currentUser();
    if (!u) return "";
    else return configPath(u);
}

QDir Ramses::currentUserCondigDir() const
{
    return createPath(currentUserConfigPath());
}

QString Ramses::defaultUserPath(RamUser *u) const
{
    return usersPath() + "/" + u->shortName();
}

QString Ramses::projectsPath() const
{
    return ramsesPath() + "/Projects";
}

QString Ramses::configPath() const
{
    return ramsesPath() + "/Config";
}

QString Ramses::path(RamProject *p) const
{
    if (!p) return pathFromRamses("");

    QString path = p->folderPath();
    if (path == "" || path == "auto") path = projectsPath() + "/" + p->shortName();
    return pathFromRamses( path );
}

QDir Ramses::dir(RamProject *p) const
{
    return createPath(path(p));
}

QString Ramses::configPath(RamProject *p) const
{
    return path(p) + "/Config";
}

QDir Ramses::configDir(RamProject *p) const
{
    return createPath(configPath(p));
}

QString Ramses::adminPath(RamProject *p) const
{
    return path(p) + "/" + "00-ADMIN";
}

QDir Ramses::adminDir(RamProject *p) const
{
    return createPath(adminPath(p));
}

QString Ramses::preProdPath(RamProject *p) const
{
    return path(p) + "/" + "01-PRE-PROD";
}

QDir Ramses::preProdDir(RamProject *p) const
{
    return createPath(preProdPath(p));
}

QString Ramses::prodPath(RamProject *p) const
{
    return path(p) + "/" + "02-PROD";
}

QDir Ramses::prodDir(RamProject *p) const
{
    return createPath(prodPath(p));
}

QString Ramses::postProdPath(RamProject *p) const
{
    return path(p) + "/" + "03-POST-PROD";
}

QDir Ramses::postProdDir(RamProject *p) const
{
    return createPath(postProdPath(p));
}

QString Ramses::assetsPath(RamProject *p) const
{
    return path(p) + "/" + "04-ASSETS";
}

QDir Ramses::assetsDir(RamProject *p) const
{
    return createPath(assetsPath(p));
}

QString Ramses::shotsPath(RamProject *p) const
{
    return path(p) + "/" + "05-SHOTS";
}

QDir Ramses::shotsDir(RamProject *p) const
{
    return createPath(shotsPath(p));
}

QString Ramses::exportPath(RamProject *p) const
{
    return path(p) + "/" + "06-EXPORT";
}

QDir Ramses::exportDir(RamProject *p) const
{
    return createPath(exportPath(p));
}

QString Ramses::path(RamStep *s) const
{
    RamProject *p = project( s->projectUuid() );
    if (!p) return "";
    if (s->type() == RamStep::PreProduction)
    {
        return preProdPath(p) + "/" + p->shortName() + "_" + s->shortName();
    }

    if (s->type() == RamStep::PostProduction)
    {
        return postProdPath(p) + "/" + p->shortName() + "_" + s->shortName();
    }

    return prodPath(p) + "/" + p->shortName() + "_" + s->shortName();
}

QDir Ramses::dir(RamStep *s) const
{
    return createPath(path(s));
}

QString Ramses::path(RamAssetGroup *ag) const
{
    if(ag->isTemplate()) return "";
    RamProject *p = project( ag->projectUuid() );
    if (!p) return "";
    return assetsPath(p) + "/" + ag->name();
}

QDir Ramses::dir(RamAssetGroup *ag) const
{
    return createPath(path(ag));
}

QString Ramses::path(RamAsset *a) const
{
    RamAssetGroup *ag = assetGroup( a->assetGroupUuid() );
    if (!ag) return "";
    RamProject *p = project( ag->projectUuid() );
    if (!p) return "";
    return path(ag) + "/" + p->shortName() + "_A_" + a->shortName();
}

QDir Ramses::dir(RamAsset *a) const
{
    return createPath(path(a));
}

QString Ramses::path(RamShot *s) const
{
    RamSequence *seq = sequence(s->sequenceUuid());
    if (!seq) return "";
    RamProject *p = project( seq->projectUuid() );
    if (!p) return "";
    return shotsPath(p) + "/" + p->shortName() + "_S_" + s->name();
}

QDir Ramses::dir(RamShot *s) const
{
    return createPath(path(s));
}

QString Ramses::defaultProjectPath(RamProject *p) const
{
    return projectsPath() + "/" + p->shortName();
}

RamProject *Ramses::currentProject() const
{
    return _currentProject;
}

void Ramses::setCurrentProject(RamProject *currentProject)
{
    _currentProject = currentProject;
    if (_currentProject)
    {
        _userSettings->setValue("currentProject", _currentProject->uuid() );
        //Update
        refresh();
    }

    emit projectChanged(_currentProject);
}

void Ramses::setCurrentProject(QString uuidOrShortName)
{
    setCurrentProject( project(uuidOrShortName) );
}

QList<RamStep *> Ramses::templateSteps() const
{
    return _templateSteps;
}

RamStep *Ramses::createTemplateStep()
{
    RamStep *step = new RamStep("New", "Step", true);
    step->setParent(this);
    _templateSteps << step;
    emit newTemplateStep(step);
    return step;
}

void Ramses::removeTemplateStep(QString uuid)
{
    for (int i = _templateSteps.count() -1; i >= 0; i--)
    {
        if (_templateSteps[i]->uuid() == uuid)
        {
            RamStep *s = _templateSteps.takeAt(i);
            s->remove();
        }
    }
}

RamStep *Ramses::templateStep(QString uuid)
{
    foreach(RamStep *step, _templateSteps)
    {
        if (step->uuid() == uuid) return step;
    }
    return nullptr;
}

QList<RamAssetGroup *> Ramses::templateAssetGroups() const
{
    return _templateAssetGroups;
}

RamAssetGroup *Ramses::createTemplateAssetGroup()
{
    RamAssetGroup *ag = new RamAssetGroup("NEW", "Asset Group", true);
    ag->setParent(this);
    _templateAssetGroups << ag;
    emit newTemplateAssetGroup(ag);
    return ag;
}

void Ramses::removeTemplateAssetGroup(QString uuid)
{
    for (int i = _templateAssetGroups.count() -1; i >= 0; i--)
    {
        if (_templateAssetGroups[i]->uuid() == uuid)
        {
            RamAssetGroup *ag = _templateAssetGroups.takeAt(i);
            ag->remove();
        }
    }
}

RamAssetGroup *Ramses::templateAssetGroup(QString uuid)
{
    foreach(RamAssetGroup *ag, _templateAssetGroups)
    {
        if (ag->uuid() == uuid) return ag;
    }
    return nullptr;
}

RamAssetGroup *Ramses::assetGroup(QString uuid) const
{
    foreach(RamProject *p, _projects)
    {
        foreach(RamAssetGroup *ag, p->assetGroups())
        {
            if (ag->uuid() == uuid) return ag;
        }
    }
    return nullptr;
}

RamSequence *Ramses::sequence(QString uuid) const
{
    foreach(RamProject *p, _projects)
    {
        foreach(RamSequence *s, p->sequences())
        {
            if (s->uuid() == uuid) return s;
        }
    }
    return nullptr;
}

QList<RamState *> Ramses::states() const
{
    return _states;
}

RamState *Ramses::createState()
{
    RamState *state = new RamState("New", "State");
    state->setParent(this);
    _states << state;
    connect(state, SIGNAL(removed(RamObject*)), this, SLOT(removeState(RamObject*)));
    emit newState(state);
    return state;
}

void Ramses::removeState(RamObject *s)
{
    if (s) removeState(s->uuid());
}

void Ramses::removeState(QString uuid)
{
    for (int i = _states.count() -1; i >= 0; i--)
    {
        if (_states[i]->uuid() == uuid)
        {
            RamState *s = _states.takeAt(i);
            s->remove();
        }
    }
}

QList<RamFileType *> Ramses::fileTypes() const
{
    return _fileTypes;
}

RamFileType *Ramses::fileType(const QString uuid) const
{
    foreach(RamFileType *ft, _fileTypes)
    {
        if (ft->uuid() == uuid) return ft;
    }
    return nullptr;
}

RamFileType *Ramses::createFileType()
{
    RamFileType *ft = new RamFileType(".new", "New File Type", QStringList("new"));
    ft->setParent(this);
    _fileTypes << ft;
    connect(ft, SIGNAL(removed(RamObject*)), this, SLOT(removeFileType(RamObject*)));
    emit newFileType(ft);
    return ft;
}

void Ramses::removeFileType(QString uuid)
{
    for (int i = _fileTypes.count() -1; i >= 0; i--)
    {
        if (_fileTypes[i]->uuid() == uuid)
        {
            RamFileType *ft = _fileTypes.takeAt(i);
            ft->remove();
        }
    }
}

void Ramses::removeFileType(RamObject *ft)
{
    if (ft) removeFileType(ft->uuid());
}

QList<RamApplication *> Ramses::applications() const
{
    return _applications;
}

RamApplication *Ramses::application(QString uuid)
{
    foreach(RamApplication *app, _applications)
    {
        if (app->uuid() == uuid) return app;
    }
    return nullptr;
}

RamApplication *Ramses::createApplication()
{
    RamApplication *a = new RamApplication("New", "New Application");
    a->setParent(this);
    _applications << a;
    connect(a, SIGNAL(removed(RamObject*)), this, SLOT(removeApplication(RamObject*)));
    emit newApplication(a);
    return a;
}

void Ramses::removeApplication(QString uuid)
{
    for (int i = _applications.count() -1; i >= 0; i--)
    {
        if (_applications[i]->uuid() == uuid)
        {
            RamApplication *a = _applications.takeAt(i);
            a->remove();
        }
    }
}

void Ramses::removeApplication(RamObject *a)
{
    if (a) removeApplication(a->uuid());
}

QList<RamProject *> Ramses::projects() const
{
    return _projects;
}

RamProject *Ramses::project(QString uuidOrShortName) const
{
    for(RamProject *p: _projects)
    {
        if (p->uuid() == uuidOrShortName) return p;
    }
    for (RamProject *p: _projects)
    {
        if (p->shortName() == uuidOrShortName) return p;
    }
    return nullptr;
}

RamProject *Ramses::createProject()
{
    RamProject *project = new RamProject("New","Project");
    project->setParent(this);
    _projects << project;
    emit newProject(project);
    return project;
}

void Ramses::removeProject(QString uuid)
{
    for (int i = _projects.count() -1; i >= 0; i--)
    {
        if (_projects[i]->uuid() == uuid)
        {
            RamProject *p = _projects.takeAt(i);
            p->remove();
        }
    }
}

RamUser *Ramses::createUser()
{
    RamUser *user = new RamUser("New","J. Doe");
    user->setParent(this);
    _users << user;
    emit newUser(user);
    return user;
}

RamUser *Ramses::user(QString uuid)
{
    foreach(RamUser *user, _users)
    {
        if (user->uuid() == uuid) return user;
    }
    return nullptr;
}

void Ramses::removeUser(QString uuid)
{
    for (int i = _users.count() -1; i >= 0; i--)
    {
        if (_users[i]->uuid() == uuid)
        {
            RamUser *u = _users.takeAt(i);
            u->remove();
        }
    }
}

bool Ramses::isAdmin()
{
    if (!_currentUser) return false;
    return _currentUser->role() >= RamUser::Admin;
}

bool Ramses::isProjectAdmin()
{
    if (!_currentUser) return false;
    return _currentUser->role() >= RamUser::ProjectAdmin;
}

bool Ramses::isLead()
{
    if (!_currentUser) return false;
    return _currentUser->role() >= RamUser::Lead;
}

QString Ramses::currentUserSettingsFile()
{
    QString settingsPath = currentUserConfigPath();
    if (settingsPath == "") return "";
    return settingsPath + "/" + "ramses.ini";
}

void Ramses::logout()
{
    _connected = false;
    _currentUser = nullptr;
    emit loggedOut();
}

void Ramses::refresh()
{
    // Get Users
    _dbi->getUsers();
    // Get Template Steps
    _dbi->getTemplateSteps();
    // Get Template Asset Groups
    _dbi->getTemplateAssetGroups();
    // Get States
    _dbi->getStates();
    // Get file types
    _dbi->getFileTypes();
    // Get applications
    _dbi->getApplications();
    // Get current project
    if (_currentProject) _dbi->getProject(_currentProject->uuid());
    else _dbi->getProjects();
}

bool Ramses::isConnected() const
{
    return _connected;
}

RamUser *Ramses::currentUser() const
{
    return _currentUser;
}

QList<RamUser *> Ramses::users() const
{
    return _users;
}
