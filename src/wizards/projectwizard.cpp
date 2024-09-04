#include "projectwizard.h"

#include <QLabel>

#include "duapp/duui.h"
#include "duapp/dusettings.h"
#include "ramjsonstepeditwidget.h"

ProjectWizard::ProjectWizard(bool team, QWidget *parent, Qt::WindowFlags flags):
    QWizard(parent, flags),
    _isTeamProject(team)
{
    setupUi();
    connectEvents();
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
    addPage(
        createPathsPage()
        );
    addPage(
        createProjectSettingsPage()
        );
    addPage(
        createPipelinePage()
        );
}

void ProjectWizard::connectEvents()
{
    connect(ui_stepList, &DuListModelEdit::editing,
            this, &ProjectWizard::editStep);
}

QWizardPage *ProjectWizard::createPathsPage()
{
    auto page = new QWizardPage();
    page->setTitle(tr("Project location"));
    page->setSubTitle(tr("Set the path to the Ramses project file, and the location where the working files are stored."));

    auto layout = DuUI::createFormLayout();
    DuUI::centerLayout(layout, page);

    ui_ramsesFileSelector = new DuFolderSelectorWidget(DuFolderSelectorWidget::File, this);
    ui_ramsesFileSelector->setPlaceHolderText(tr("File path of the Ramses Database..."));
    ui_ramsesFileSelector->setDialogTitle(tr("Select the location of the Ramses Database."));
    ui_ramsesFileSelector->setMode(DuFolderSelectorWidget::Save);
    ui_ramsesFileSelector->setFilter(tr("Ramses (*.ramses);;SQLite (*.sqlite);;All Files (*.*)"));
    ui_ramsesFileSelector->setMinimumWidth(300);
    layout->addRow(tr("Ramses project file"), ui_ramsesFileSelector);

    ui_projectPathSelector = new DuFolderSelectorWidget();
    ui_projectPathSelector->showRevealButton(false);
    ui_projectPathSelector->setPlaceHolderText(QDir::homePath() + "/Ramses Project");
    ui_projectPathSelector->setMinimumWidth(300);
    layout->addRow(tr("Project working directory"), ui_projectPathSelector);

    return page;
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
