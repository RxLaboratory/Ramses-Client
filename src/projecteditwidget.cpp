#include "projecteditwidget.h"

ProjectEditWidget::ProjectEditWidget(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);

    connect(updateButton, SIGNAL(clicked()), this, SLOT(update()));
    connect(revertButton, SIGNAL(clicked()), this, SLOT(revert()));
    connect(shortNameEdit, &QLineEdit::textChanged, this, &ProjectEditWidget::checkInput);
    connect(DBInterface::instance(),&DBInterface::log, this, &ProjectEditWidget::dbiLog);


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
    this->setEnabled(false);

    if(!project) return;

    nameEdit->setText(project->name());
    shortNameEdit->setText(project->shortName());

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

void ProjectEditWidget::projectDestroyed(QObject */*o*/)
{
    setProject(nullptr);
}

void ProjectEditWidget::dbiLog(QString m, LogUtils::LogType t)
{
    if (t != LogUtils::Remote && t != LogUtils::Debug) statusLabel->setText(m);
}
