#include "projectwizard.h"

#include <QLabel>

#include "duapp/duui.h"
#include "duapp/dusettings.h"
#include "duutils/dusystemutils.h"
#include "projectmanager.h"
#include "ramnamemanager.h"
#include "ramuser.h"
#include "ramproject.h"
#include "ramstep.h"
#include "ramses.h"
#include "ramjsonstepeditwidget.h"

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

    // Create the database and project
    QString dbPath = ui_pathsPage->dbFilePath();

    // Create the file
    if (!ProjectManager::i()->createDatabase(
        dbPath,
        ui_pathsPage->projectPath())
        ) {
        QMessageBox::warning(this,
                             tr("Can't save the database"),
                             tr("Sorry, the database creation failed at this location.\nMaybe you can try another location...") + "\n\n" +
                                 dbPath
                             );
        return QWizard::done(r);
    }

    // Set the file
    ProjectManager::i()->loadDatabase(dbPath);

    // Create a user
    QString username = SystemUtils::userName();
    RamUser *user = new RamUser(
        RamNameManager::nameToShortName(username),
        username
        );
    user->setRole(RamUser::Admin);

    // Create project
    RamProject *project = new RamProject(
        ui_detailsPage->shortName(),
        ui_detailsPage->name()
        );
    // Set the project details
    project->setWidth(ui_resolutionWidget->getWidth());
    project->setHeight(ui_resolutionWidget->getHeight());
    project->setFramerate(ui_framerateWidget->framerate());
    project->setDeadline(ui_deadlineEdit->date());
    // Assign the user
    project->users()->appendObject(user->uuid());

    // Create the steps
    const QVector<QJsonObject> &jsonSteps = _steps->objects();
    for(const auto &jsonStep: jsonSteps)
        RamStep::fromJson(jsonStep, project);

    // Login
    Ramses::instance()->setUser( user );

    QWizard::done(r);
}

void ProjectWizard::editStep(const QModelIndex &index)
{
    auto editor = new RamJsonStepEditWidget(this);
    editor->setData(index.data(Qt::EditRole).toJsonObject());

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

void ProjectWizard::setupUi()
{
    ui_detailsPage = new RamObjectPropertiesWizardPage(this);
    addPage( ui_detailsPage );

    addPage(
        createProjectSettingsPage()
        );
    addPage(
        createPipelinePage()
        );

    ui_pathsPage = new RamProjectPathsPage(this);
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
