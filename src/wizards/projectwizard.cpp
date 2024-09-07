#include "projectwizard.h"

#include <QLabel>

#include "duapp/app-config.h"
#include "duapp/duui.h"
#include "duapp/dusettings.h"
#include "duutils/dusystemutils.h"
#include "projectmanager.h"
#include "ramnamemanager.h"
#include "ramstep.h"
#include "ramses.h"
#include "ramjsonstepeditwidget.h"
#include "ramserverclient.h"

ProjectWizard::ProjectWizard(bool team, QWidget *parent, Qt::WindowFlags flags):
    QWizard(parent, flags),
    _isTeamProject(team)
{
    setupUi();
    connectEvents();
}

void ProjectWizard::done(int r)
{
    if (r != QWizard::Accepted)
        return QWizard::done(r);

    if (_isTeamProject)
        _userUuid = ui_loginPage->uuid();

    // Create distant data
    // (project, users and assignemnts)
    if (_isTeamProject) {
        _projectUuid = createServerData();
        if (_projectUuid == "")
            return;
    }

    // Create the local database
    QString dbPath = ui_pathsPage->dbFilePath();

    // Remove existing file
    if (!askRemoveExistingFile(dbPath))
        return;

    // Create the file
    if (!createDatabase(dbPath))
        return;

    // Disable while we finish the setup
    this->setEnabled(false);

    // Set the file
    ProjectManager::i()->loadDatabase(dbPath);

    // Create local data
    if (_isTeamProject) {
        // Set the server settings in the DB
        LocalDataInterface::instance()->setServerSettings(dbPath, RamServerClient::i()->serverConfig());

        // Download distant data
        RamServerClient::i()->setProject(_projectUuid);
        // Just wait for it
        connect(DBInterface::i(), &DBInterface::syncFinished,
                this, &ProjectWizard::finishProjectSetup);
        RamServerClient::i()->downloadAllData();
        return;
    }

    RamUser *user = createLocalUser();
    _userUuid = user->uuid();
    // Project
    RamProject *project = createLocalProject();
    // Assign the user
    project->users()->appendObject(user->uuid());
    _projectUuid = project->uuid();

    finishProjectSetup();
}

void ProjectWizard::editStep(const QModelIndex &index)
{
    auto editor = new RamJsonStepEditWidget(this);

    connect(editor, &RamJsonStepEditWidget::dataChanged,
            this, [this, index] (const QJsonObject &obj) {
        _steps->setData(index, obj);
    });
    connect(this, &ProjectWizard::destroyed,
            editor, &RamJsonStepEditWidget::deleteLater);

    // Show
    DuUI::appMainWindow()->setPropertiesDockWidget(
        editor,
        tr("Step"),
        ":/icons/step",
        true);
}

void ProjectWizard::finishProjectSetup()
{
    disconnect(DBInterface::i(), nullptr, this, nullptr);

    RamProject *project = RamProject::get(_projectUuid);
    if (!project) {
        QMessageBox::warning(
            this,
            tr("Project creation failed."),
            tr("Something went wrong while downloading data from the server, sorry.")
            );
        this->setEnabled(true);
        return;
    }

    RamUser *user = RamUser::get(_userUuid);
    if (!user) {
        QMessageBox::warning(
            this,
            tr("Project creation failed."),
            tr("Something went wrong while downloading data from the server, sorry.")
            );
        this->setEnabled(true);
        return;
    }

    // Set the project in the DB
    LocalDataInterface::instance()->setCurrentProjectUser(_projectUuid, _userUuid);

    // Create the steps
    const QVector<QJsonObject> &jsonSteps = _steps->objects();
    for(const auto &jsonStep: jsonSteps)
        RamStep::fromJson(jsonStep, project);

    // Login
    Ramses::i()->setUser( user );

    // and accept
    this->setEnabled(true);
    QWizard::done(QWizard::Accepted);
}

void ProjectWizard::setupUi()
{
    if (_isTeamProject) {
        ui_loginPage = new LoginWizardPage(this);
        addPage( ui_loginPage );
    }

    ui_detailsPage = new RamObjectPropertiesWizardPage(this);
    addPage( ui_detailsPage );

    addPage(
        createProjectSettingsPage()
        );
    addPage(
        createPipelinePage()
        );

    ui_pathsPage = new RamDatabasePathsWizardPage(this);
    addPage( ui_pathsPage );
}

void ProjectWizard::connectEvents()
{
    connect(ui_stepList, &DuListModelEdit::editing,
            this, &ProjectWizard::editStep);
}

QWizardPage *ProjectWizard::createProjectSettingsPage()
{
    auto page = new QWizardPage();
    page->setTitle(tr("Project settings"));
    page->setSubTitle(tr("Set the details of the project."));

    auto layout = DuUI::createFormLayout();
    DuUI::centerLayout(layout, page);
    int m = DuSettings::i()->get(DuSettings::UI_Margins).toInt();
    layout->setSpacing(m*3);

    ui_resolutionWidget = new ResolutionWidget(this);
    layout->addRow(tr("Delivery resolution"), ui_resolutionWidget);

    ui_framerateWidget = new FramerateWidget(this);
    layout->addRow(tr("Delivery framerate"), ui_framerateWidget);

    ui_parBox = new QDoubleSpinBox(this);
    layout->addRow(tr("Delivery pixel aspect ratio"), ui_parBox);
    ui_parBox->setMinimum(0.01);
    ui_parBox->setMaximum(10.0);
    ui_parBox->setDecimals(2);
    ui_parBox->setValue(1.0);

    ui_deadlineEdit = new QDateEdit(this);
    ui_deadlineEdit->setCalendarPopup(true);
    ui_deadlineEdit->setDate( QDate::currentDate() );
    layout->addRow(tr("Deadline"), ui_deadlineEdit);

    return page;
}

QWizardPage *ProjectWizard::createPipelinePage()
{
    auto page = new QWizardPage();
    page->setTitle(tr("Production pipeline setup"));
    page->setSubTitle(tr("To get started, let's add some production steps in the new project pipeline."));

    auto layout = DuUI::createBoxLayout(Qt::Vertical);
    DuUI::centerLayout(layout, page, 200);

    _steps = new RamJsonObjectModel(this);
    ui_stepList = new DuListModelEdit(tr("Production steps"), _steps, this);
    layout->addWidget(ui_stepList);

    return page;
}

bool ProjectWizard::askRemoveExistingFile(const QString &dbPath)
{
    if (QFileInfo::exists(dbPath)) {
        QMessageBox::StandardButton ok = QMessageBox::question(
            this,
            tr("Confirm file overwrite"),
            tr("Are you sure you want to overwrite this file?") + "\n\n" + dbPath
            );
        if (ok != QMessageBox::Yes)
            return false;

        FileUtils::remove(dbPath);
    }
    return true;
}

bool ProjectWizard::createDatabase(const QString &dbPath)
{
    if (!LocalDataInterface::createNewDatabase( dbPath ) )
    {
        QMessageBox::warning(this,
                             tr("Can't save the database"),
                             tr("Sorry, the database creation failed at this location.\nMaybe you can try another location...") + "\n\n" +
                                 dbPath
                             );
        return false;
    }
    LocalDataInterface::setWorkingPath( dbPath, ui_pathsPage->projectPath() );

    return true;
}

QString ProjectWizard::createServerData()
{
    auto rsc = RamServerClient::i();

    // Create project

    QJsonObject projectData;
    projectData.insert(RamProject::KEY_ShortName, ui_detailsPage->shortName());
    projectData.insert(RamProject::KEY_Name, ui_detailsPage->name());
    projectData.insert(RamProject::KEY_Width, ui_resolutionWidget->getWidth());
    projectData.insert(RamProject::KEY_Height, ui_resolutionWidget->getHeight());
    projectData.insert(RamProject::KEY_FrameRate, ui_framerateWidget->framerate());
    projectData.insert(RamProject::KEY_Deadline, ui_deadlineEdit->date().toString(DATE_DATA_FORMAT));
    projectData.insert(RamProject::KEY_PixelAspectRatio, ui_parBox->value());

    // Add the user list
    // At least ourselves
    QJsonArray usersArr;
    usersArr.append(_userUuid);
    projectData.insert(RamProject::KEY_Users, usersArr);

    QJsonDocument doc;
    doc.setObject(projectData);

    QJsonObject rep = rsc->createProject(doc.toJson(QJsonDocument::Compact));
    if (!checkServerReply(rep))
        return "";

    QString projectUuid = rep.value("content").toString();

    // Create users

    // Assign users on the server
    // At least ourselves
    rep = rsc->assignUsers( { _userUuid } , projectUuid);
    if (!checkServerReply(rep))
        return "";

    return projectUuid;
}

bool ProjectWizard::checkServerReply(const QJsonObject &reply)
{
    if (!reply.value("success").toBool(false)) {
        QMessageBox::warning(
            this,
            tr("Ramses team project creation failed"),
            reply.value("message").toString("Unknown error")
            );
        return false;
    }
    return true;
}

RamUser *ProjectWizard::createLocalUser()
{
    QString username = SystemUtils::userName();
    RamUser *user = new RamUser(
        RamNameManager::nameToShortName(username),
        username
        );
    return user;
}

RamProject *ProjectWizard::createLocalProject()
{
    RamProject *project = new RamProject(
        ui_detailsPage->shortName(),
        ui_detailsPage->name()
        );
    // Set the project details
    project->setWidth(ui_resolutionWidget->getWidth());
    project->setHeight(ui_resolutionWidget->getHeight());
    project->setFramerate(ui_framerateWidget->framerate());
    project->setDeadline(ui_deadlineEdit->date());
    project->setPixelAspectRatio(ui_parBox->value());

    return project;
}
