#include "applicationeditwidget.h"

ApplicationEditWidget::ApplicationEditWidget(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);

    folderSelector = new DuQFFolderSelectorWidget(DuQFFolderSelectorWidget::File, this);
    executableLayout->addWidget(folderSelector);

    connect(updateButton, SIGNAL(clicked()), this, SLOT(update()));
    connect(revertButton, SIGNAL(clicked()), this, SLOT(revert()));
    connect(shortNameEdit, &QLineEdit::textChanged, this, &ApplicationEditWidget::checkInput);
    connect(DBInterface::instance(),&DBInterface::newLog, this, &ApplicationEditWidget::dbiLog);

    this->setEnabled(false);
}

RamApplication *ApplicationEditWidget::application() const
{
    return _application;
}

void ApplicationEditWidget::setApplication(RamApplication *application)
{
    disconnect(_currentApplicationConnection);

    _application = application;

    nameEdit->setText("");
    shortNameEdit->setText("");
    folderSelector->setPath("");

    if (!application) return;

    nameEdit->setText(application->name());
    shortNameEdit->setText(application->shortName());
    folderSelector->setPath(application->executableFilePath());

    this->setEnabled(Ramses::instance()->isProjectAdmin());

    _currentApplicationConnection = connect(application, &RamObject::removed, this, &ApplicationEditWidget::applicationRemoved);
}

void ApplicationEditWidget::update()
{
    if (!_application) return;

    this->setEnabled(false);

    if (!checkInput())
    {
        this->setEnabled(true);
        return;
    }

    _application->setName(nameEdit->text());
    _application->setShortName(shortNameEdit->text());
    _application->setExecutableFilePath(folderSelector->path());

    _application->update();

    this->setEnabled(true);
}

void ApplicationEditWidget::revert()
{
    setApplication(_application);
}

bool ApplicationEditWidget::checkInput()
{
    if (!_application) return false;

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

void ApplicationEditWidget::applicationRemoved(RamObject *o)
{
    Q_UNUSED(o);
    setApplication(nullptr);
}

void ApplicationEditWidget::dbiLog(DuQFLog m)
{
    if (m.type() != DuQFLog::Debug) statusLabel->setText(m.message());
}
