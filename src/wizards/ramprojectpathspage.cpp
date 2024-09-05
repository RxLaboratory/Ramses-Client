#include "ramprojectpathspage.h"

#include "duapp/duui.h"

RamProjectPathsPage::RamProjectPathsPage(QWidget *parent):
    QWizardPage(parent)
{
    setupUi();
    connectEvents();
}

bool RamProjectPathsPage::isComplete() const
{
    return ui_ramsesFileSelector->path() != "" &&
           QFileInfo::exists(ui_projectPathSelector->path());
}

bool RamProjectPathsPage::validatePage()
{
    if (!isComplete()) {
        QMessageBox::warning(this,
                             tr("Incomplete data"),
                             tr("Sorry, you must choose where to save the project and select a working folder.")
                             );
        return false;
    }

    // Remove existing file
    QString filePath = dbFilePath();
    if (!QFileInfo::exists(filePath))
        return true;

    QMessageBox::StandardButton ok = QMessageBox::question(
        this,
        tr("Confirm file overwrite"),
        tr("Are you sure you want to overwrite this file?") + "\n\n" + filePath
        );
    if (ok != QMessageBox::Yes)
        return false;

    FileUtils::remove(filePath);
    return true;
}

void RamProjectPathsPage::initializePage()
{
    QString id = field("shortName").toString();
    QString name = field("name").toString();

    ui_ramsesFileSelector->setPath(QDir::homePath() + "/" + id + ".ramses");
    ui_projectPathSelector->setPlaceHolderText(QDir::homePath() + "/" + id + "_" + name);
}

void RamProjectPathsPage::setupUi()
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

void RamProjectPathsPage::connectEvents()
{
    connect(ui_ramsesFileSelector, &DuFolderSelectorWidget::pathChanged,
            this, &RamProjectPathsPage::completeChanged);
    connect(ui_projectPathSelector, &DuFolderSelectorWidget::pathChanged,
            this, &RamProjectPathsPage::completeChanged);
}
