#include "ramdatabasepathswizardpage.h"

#include "duapp/duui.h"

RamDatabasePathsWizardPage::RamDatabasePathsWizardPage(QWidget *parent):
    QWizardPage(parent)
{
    setupUi();
    connectEvents();
}

bool RamDatabasePathsWizardPage::isComplete() const
{
    return ui_ramsesFileSelector->path() != "" &&
           QFileInfo::exists(ui_projectPathSelector->path());
}

bool RamDatabasePathsWizardPage::validatePage()
{
    if (!isComplete()) {
        QMessageBox::warning(this,
                             tr("Incomplete data"),
                             tr("Sorry, you must choose where to save the project and select a working folder.")
                             );
        return false;
    }

    return true;
}

void RamDatabasePathsWizardPage::initializePage()
{
    QString id = field("shortName").toString();
    QString name = field("name").toString();

    ui_ramsesFileSelector->setPath(QDir::homePath() + "/" + id + ".ramses");
    ui_projectPathSelector->setPlaceHolderText(QDir::homePath() + "/" + id + "_" + name);
}

void RamDatabasePathsWizardPage::setupUi()
{
    this->setTitle(tr("Project location"));
    this->setSubTitle(tr("Set the path to the Ramses project file, and the location where the working files are stored."));

    auto layout = DuUI::createFormLayout();
    DuUI::centerLayout(layout, this);

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
}

void RamDatabasePathsWizardPage::connectEvents()
{
    connect(ui_ramsesFileSelector, &DuFolderSelectorWidget::pathChanged,
            this, &RamDatabasePathsWizardPage::completeChanged);
    connect(ui_projectPathSelector, &DuFolderSelectorWidget::pathChanged,
            this, &RamDatabasePathsWizardPage::completeChanged);
}
