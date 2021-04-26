#include "projecteditwidget.h"

ProjectEditWidget::ProjectEditWidget(QWidget *parent) :
    ObjectEditWidget(parent)
{
    setupUi();
    connectEvents();

    setProject(nullptr);

    this->setEnabled(false);
}

ProjectEditWidget::ProjectEditWidget(RamProject *project, QWidget *parent) :
    ObjectEditWidget(project, parent)
{
    setupUi();
    connectEvents();

    setProject(project);
}

RamProject *ProjectEditWidget::project() const
{
    return _project;
}

void ProjectEditWidget::setProject(RamProject *project)
{
    this->setEnabled(false);

    setObject(project);
    _project = project;

    QSignalBlocker b1(folderSelector);
    QSignalBlocker b2(resolutionWidget);
    QSignalBlocker b3(framerateWidget);

    //Reset values
    resolutionWidget->setHeight(1080);
    resolutionWidget->setWidth(1920);
    framerateWidget->setFramerate(24.0);
    folderSelector->setPath("");
    folderSelector->setPlaceHolderText("Default (Ramses/Projects/Project_ShortName)");

    if(!project) return;

    resolutionWidget->setHeight(project->height());
    resolutionWidget->setWidth(project->width());
    framerateWidget->setFramerate(project->framerate());

    if (project->folderPath() != "auto") folderSelector->setPath( project->folderPath() );
    folderSelector->setPlaceHolderText( Ramses::instance()->defaultProjectPath(project) );
    folderLabel->setText( Ramses::instance()->path(project) );

    _objectConnections << connect(project,&RamProject::changed, this, &ProjectEditWidget::projectChanged);

    this->setEnabled( Ramses::instance()->isAdmin() ); 
}

void ProjectEditWidget::update()
{
    if (!_project) return;

    updating = true;

    _project->setFolderPath( folderSelector->path() );
    _project->setWidth( resolutionWidget->getWidth() );
    _project->setHeight( resolutionWidget->getHeight() );
    _project->setFramerate( framerateWidget->framerate() );

    ObjectEditWidget::update();

    updating = false;
}

void ProjectEditWidget::updateFolderLabel(QString path)
{
    if (path != "") folderLabel->setText( Ramses::instance()->pathFromRamses(path));
    else if (_project) folderLabel->setText( Ramses::instance()->path(_project) );
}

void ProjectEditWidget::projectChanged(RamObject *o)
{
    if (updating) return;
    Q_UNUSED(o);
    setProject(_project);
}

void ProjectEditWidget::setupUi()
{
    QLabel *rLabel = new QLabel("Resolution", this);
    rLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    mainFormLayout->addWidget(rLabel, 2, 0);

    resolutionWidget = new ResolutionWidget(this);
    mainFormLayout->addWidget(resolutionWidget, 2, 1);

    QLabel *frLabel = new QLabel("Framerate", this);
    frLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    mainFormLayout->addWidget(frLabel, 3, 0);

    framerateWidget = new FramerateWidget(this);
    mainFormLayout->addWidget(framerateWidget, 3, 1);

    QLabel *fLabel = new QLabel("Folder", this);
    mainFormLayout->addWidget(fLabel, 4, 0);

    folderSelector = new DuQFFolderSelectorWidget(DuQFFolderSelectorWidget::Folder, this);
    folderSelector->setPlaceHolderText("Default (Ramses/Users/User_ShortName)");
    mainFormLayout->addWidget(folderSelector, 4, 1);

    folderLabel = new QLabel("", this);
    mainFormLayout->addWidget(folderLabel, 5, 1);

    mainLayout->addStretch();
}

void ProjectEditWidget::connectEvents()
{
    connect(resolutionWidget, &ResolutionWidget::resolutionChanged, this, &ProjectEditWidget::update);
    connect(framerateWidget, &FramerateWidget::framerateChanged, this, &ProjectEditWidget::update);
    connect(folderSelector, &DuQFFolderSelectorWidget::pathChanging, this, &ProjectEditWidget::updateFolderLabel);
    connect(folderSelector, &DuQFFolderSelectorWidget::pathChanged, this, &ProjectEditWidget::update);
}
