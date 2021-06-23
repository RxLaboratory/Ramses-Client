#include "projecteditwidget.h"

ProjectEditWidget::ProjectEditWidget(QWidget *parent) :
    ObjectEditWidget(parent)
{
    setupUi();
    connectEvents();

    setObject(nullptr);

    this->setEnabled(false);
}

ProjectEditWidget::ProjectEditWidget(RamProject *project, QWidget *parent) :
    ObjectEditWidget(project, parent)
{
    setupUi();
    connectEvents();

    setObject(project);
}

RamProject *ProjectEditWidget::project() const
{
    return _project;
}

void ProjectEditWidget::setObject(RamObject *obj)
{
    RamProject *project = (RamProject*)obj;

    this->setEnabled(false);

    ObjectEditWidget::setObject(project);
    _project = project;

    QSignalBlocker b1(ui_folderSelector);
    QSignalBlocker b2(ui_resolutionWidget);
    QSignalBlocker b3(ui_framerateWidget);

    //Reset values
    ui_resolutionWidget->setHeight(1080);
    ui_resolutionWidget->setWidth(1920);
    ui_framerateWidget->setFramerate(24.0);
    ui_folderSelector->setPath("");
    ui_folderSelector->setPlaceHolderText("Default (Ramses/Projects/Project_ShortName)");

    if(!project) return;

    ui_resolutionWidget->setHeight(project->height());
    ui_resolutionWidget->setWidth(project->width());
    ui_framerateWidget->setFramerate(project->framerate());

    if (project->folderPath() != "auto") ui_folderSelector->setPath( project->folderPath() );
    ui_folderSelector->setPlaceHolderText( Ramses::instance()->defaultProjectPath(project) );
    ui_folderLabel->setText( Ramses::instance()->path(project) );


    this->setEnabled( Ramses::instance()->isAdmin() ); 
}

void ProjectEditWidget::update()
{
    if (!_project) return;

    updating = true;

    _project->setFolderPath( ui_folderSelector->path() );
    _project->setWidth( ui_resolutionWidget->getWidth() );
    _project->setHeight( ui_resolutionWidget->getHeight() );
    _project->setFramerate( ui_framerateWidget->framerate() );

    ObjectEditWidget::update();

    updating = false;
}

void ProjectEditWidget::updateFolderLabel(QString path)
{
    if (path != "") ui_folderLabel->setText( Ramses::instance()->pathFromRamses(path));
    else if (_project) ui_folderLabel->setText( Ramses::instance()->path(_project) );
}

void ProjectEditWidget::currentUserChanged(RamUser *user)
{
    if (!user) return;
    QSettings *userSettings = user->userSettings();
    ui_deadlineEdit->setDisplayFormat( userSettings->value("ramses/dateFormat","yyyy-MM-dd hh:mm:ss").toString());
}

void ProjectEditWidget::setupUi()
{
    QLabel *rLabel = new QLabel("Resolution", this);
    rLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    ui_mainFormLayout->addWidget(rLabel, 2, 0);

    ui_resolutionWidget = new ResolutionWidget(this);
    ui_mainFormLayout->addWidget(ui_resolutionWidget, 2, 1);

    QLabel *frLabel = new QLabel("Framerate", this);
    frLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    ui_mainFormLayout->addWidget(frLabel, 3, 0);

    ui_framerateWidget = new FramerateWidget(this);
    ui_mainFormLayout->addWidget(ui_framerateWidget, 3, 1);

    ui_deadlineEdit = new QDateTimeEdit(this);
    ui_deadlineEdit->setCalendarPopup(true);
    ui_deadlineEdit->setDateTime( QDateTime::currentDateTime() );
    ui_mainFormLayout->addWidget(new QLabel("Deadline"), 4,0);
    ui_mainFormLayout->addWidget(ui_deadlineEdit, 4, 1);

    QLabel *fLabel = new QLabel("Folder", this);
    ui_mainFormLayout->addWidget(fLabel, 5, 0);

    ui_folderSelector = new DuQFFolderSelectorWidget(DuQFFolderSelectorWidget::Folder, this);
    ui_folderSelector->setPlaceHolderText("Default (Ramses/Users/User_ShortName)");
    ui_mainFormLayout->addWidget(ui_folderSelector, 5, 1);

    ui_folderLabel = new QLabel("", this);
    ui_mainFormLayout->addWidget(ui_folderLabel, 6, 1);

    ui_mainLayout->addStretch();
}

void ProjectEditWidget::connectEvents()
{
    connect(Ramses::instance(), SIGNAL(loggedIn(RamUser*)), this, SLOT(currentUserChanged(RamUser*)));
    connect(ui_resolutionWidget, &ResolutionWidget::resolutionChanged, this, &ProjectEditWidget::update);
    connect(ui_framerateWidget, &FramerateWidget::framerateChanged, this, &ProjectEditWidget::update);
    connect(ui_folderSelector, &DuQFFolderSelectorWidget::pathChanging, this, &ProjectEditWidget::updateFolderLabel);
    connect(ui_folderSelector, &DuQFFolderSelectorWidget::pathChanged, this, &ProjectEditWidget::update);
}
