#include "projecteditwidget.h"

ProjectEditWidget::ProjectEditWidget(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);

    folderSelector = new DuQFFolderSelectorWidget(this);
    folderSelector->setPlaceHolderText("Default (Ramses/Users/User_ShortName)");
    folderLayout->addWidget(folderSelector);

    connect(updateButton, SIGNAL(clicked()), this, SLOT(update()));
    connect(revertButton, SIGNAL(clicked()), this, SLOT(revert()));
    connect(shortNameEdit, &QLineEdit::textChanged, this, &ProjectEditWidget::checkInput);
    connect(folderSelector, &DuQFFolderSelectorWidget::pathChanging, this, &ProjectEditWidget::updateFolderLabel);
    connect(DBInterface::instance(),&DBInterface::newLog, this, &ProjectEditWidget::dbiLog);


    this->setEnabled(false);
}

RamProject *ProjectEditWidget::project() const
{
    return _project;
}

void ProjectEditWidget::setProject(RamProject *project)
{
    disconnect(_currentProjectConnection);

    _project = project;
    nameEdit->setText("");
    shortNameEdit->setText("");
    folderSelector->setPath("");
    folderSelector->setPlaceHolderText("Default (Ramses/Projects/Project_ShortName)");
    this->setEnabled(false);

    if(!project) return;

    nameEdit->setText(project->name());
    shortNameEdit->setText(project->shortName());

    if (project->folderPath() != "auto") folderSelector->setPath( project->folderPath() );
    folderSelector->setPlaceHolderText( Ramses::instance()->defaultProjectPath(project) );
    folderLabel->setText( Ramses::instance()->path(project) );


    this->setEnabled( Ramses::instance()->isAdmin() );

    _currentProjectConnection = connect(project,&RamProject::destroyed, this, &ProjectEditWidget::projectDestroyed);
}

void ProjectEditWidget::update()
{
    if (!_project) return;

    this->setEnabled(false);

    //check if everything is alright
    if (!checkInput())
    {
        this->setEnabled(true);
        return;
    }

    _project->setName(nameEdit->text());
    _project->setShortName(shortNameEdit->text());
    _project->setFolderPath( folderSelector->path() );

    _project->update();

    this->setEnabled(true);

}

void ProjectEditWidget::revert()
{
    setProject(_project);
}

bool ProjectEditWidget::checkInput()
{
    if (!_project) return false;

    if (shortNameEdit->text() == "")
    {
        statusLabel->setText("Short name cannot be empty!");
        updateButton->setEnabled(false);
        return false;
    }

    statusLabel->setText("");
    updateButton->setEnabled(true);
    return true;
}

void ProjectEditWidget::updateFolderLabel(QString path)
{
    if (path != "") folderLabel->setText( Ramses::instance()->pathFromRamses(path));
    else if (_project) folderLabel->setText( Ramses::instance()->path(_project) );
}

void ProjectEditWidget::projectDestroyed(QObject */*o*/)
{
    setProject(nullptr);
}

void ProjectEditWidget::dbiLog(DuQFLog m)
{
    if (m.type() != DuQFLog::Debug) statusLabel->setText(m.message());
}
