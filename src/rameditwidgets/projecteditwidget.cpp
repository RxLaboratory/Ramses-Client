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
    return m_project;
}

void ProjectEditWidget::setObject(RamObject *obj)
{
    RamProject *project = qobject_cast<RamProject*>(obj);

    QSignalBlocker b1(ui_folderSelector);
    QSignalBlocker b2(ui_resolutionWidget);
    QSignalBlocker b3(ui_framerateWidget);
    QSignalBlocker b4(ui_deadlineEdit);
    QSignalBlocker b5(m_userList);

    this->setEnabled(false);

    ObjectEditWidget::setObject(project);
    m_project = project;

    //Reset values
    ui_resolutionWidget->setHeight(1080);
    ui_resolutionWidget->setWidth(1920);
    ui_framerateWidget->setFramerate(24.0);
    ui_folderSelector->setPath("");
    ui_folderSelector->setPlaceHolderText("Default (Ramses/Projects/Project_ShortName)");
    ui_deadlineEdit->setDate( QDate::currentDate().addDays(30) );
    m_userList->clear();

    if(!project) return;

    ui_resolutionWidget->setHeight(project->height());
    ui_resolutionWidget->setWidth(project->width());
    ui_framerateWidget->setFramerate(project->framerate());
    ui_deadlineEdit->setDate( project->deadline() );

    if (!project->pathIsDefault()) ui_folderSelector->setPath( project->path() );
    ui_folderSelector->setPlaceHolderText( project->defaultPath() );
    QString p = project->path();
    if (p.count() > 45) p = p.replace(0, p.count()-45, "(...)");
    ui_folderLabel->setText( p );

    m_userList->setList(project->users());


    this->setEnabled( Ramses::instance()->isAdmin() );
    // Projects should be edited only by admins
    /*if (!this->isEnabled())
    {
        this->setEnabled( project->is(Ramses::instance()->currentProject()) && Ramses::instance()->isProjectAdmin() );
    }*/
}

void ProjectEditWidget::update()
{
    if (!checkInput()) return;

    updating = true;

    m_project->setFolderPath( ui_folderSelector->path() );
    m_project->setWidth( ui_resolutionWidget->getWidth() );
    m_project->setHeight( ui_resolutionWidget->getHeight() );
    m_project->setFramerate( ui_framerateWidget->framerate() );
    m_project->setDeadline( ui_deadlineEdit->date() );

    ObjectEditWidget::update();

    updating = false;
}

void ProjectEditWidget::updateFolderLabel(QString path)
{
    if (path != "") ui_folderLabel->setText( Ramses::instance()->pathFromRamses(path));
    else if (m_project) ui_folderLabel->setText( m_project->defaultPath() );
}

void ProjectEditWidget::currentUserChanged(RamUser *user)
{
    if (!user) return;
    QSettings *userSettings = user->settings();
    ui_deadlineEdit->setDisplayFormat( userSettings->value("ramses/dateFormat","yyyy-MM-dd hh:mm:ss").toString());
}

void ProjectEditWidget::createUser()
{
    if (!m_project) return;
    RamUser *user = new RamUser(
                "NEW",
                "John Doe");
    Ramses::instance()->users()->append(user);
    m_project->users()->append(user);
    user->edit();
}

void ProjectEditWidget::savePath()
{
    if (!m_project) return;

    QMessageBox::StandardButton confirm = QMessageBox::question( this,
        "Confirm overwrite",
        "This will overwrite the default path for this project, for all users.\nDo you want to continue?" );

    if ( confirm != QMessageBox::Yes) return;

    m_project->updatePath();
}

void ProjectEditWidget::reinitPath()
{
    QSignalBlocker b(m_project);
    m_project->resetDbFolderPath();
    if (!m_project->pathIsDefault()) ui_folderSelector->setPath( m_project->path() );
    else ui_folderSelector->setPath( "" );
    ui_folderSelector->setPlaceHolderText( m_project->defaultPath() );
    ui_folderLabel->setText( m_project->path() );
}

void ProjectEditWidget::setupUi()
{
    QLabel *rLabel = new QLabel("Resolution", this);
    //rLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    ui_mainFormLayout->addWidget(rLabel, 3, 0);

    ui_resolutionWidget = new ResolutionWidget(this);
    ui_mainFormLayout->addWidget(ui_resolutionWidget, 3, 1);

    QLabel *frLabel = new QLabel("Framerate", this);
    //frLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    ui_mainFormLayout->addWidget(frLabel, 4, 0);

    ui_framerateWidget = new FramerateWidget(this);
    ui_mainFormLayout->addWidget(ui_framerateWidget, 4, 1);

    ui_deadlineEdit = new QDateEdit(this);
    ui_deadlineEdit->setCalendarPopup(true);
    ui_deadlineEdit->setDate( QDate::currentDate() );
    ui_mainFormLayout->addWidget(new QLabel("Deadline"), 5,0);
    ui_mainFormLayout->addWidget(ui_deadlineEdit, 5, 1);

    QLabel *fLabel = new QLabel("Folder", this);
    ui_mainFormLayout->addWidget(fLabel, 6, 0);

    ui_folderSelector = new DuQFFolderSelectorWidget(DuQFFolderSelectorWidget::Folder, this);
    ui_folderSelector->setPlaceHolderText("Default (Ramses/Users/User_ShortName)");
    ui_mainFormLayout->addWidget(ui_folderSelector,6, 1);

    QWidget *fWidget = new QWidget();
    fWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    QHBoxLayout *fLayout = new QHBoxLayout(fWidget);
    fLayout->setSpacing(3);
    fLayout->setContentsMargins(0,0,0,0);

    ui_folderLabel = new QLabel("", this);
    fLayout->addWidget(ui_folderLabel);

    ui_savePathButton = new QToolButton(this);
    ui_savePathButton->setToolTip("Save path as default");
    ui_savePathButton->setIcon(QIcon(":/icons/save"));
    ui_savePathButton->setIconSize(QSize(16,16));
    fLayout->addWidget(ui_savePathButton);

    ui_reinitPathButton = new QToolButton(this);
    ui_reinitPathButton->setToolTip("Restore default path");
    ui_reinitPathButton->setIcon(QIcon(":/icons/reinit"));
    ui_reinitPathButton->setIconSize(QSize(16,16));
    fLayout->addWidget(ui_reinitPathButton);

    ui_mainFormLayout->addWidget(fWidget, 7, 1);

    m_userList = new ObjectListEditWidget(true, RamUser::ProjectAdmin, this);
    m_userList->setEditMode(ObjectListEditWidget::UnassignObjects);
    m_userList->setTitle("Users");
    m_userList->setAssignList(Ramses::instance()->users());
    ui_mainLayout->addWidget(m_userList);
}

void ProjectEditWidget::connectEvents()
{
    connect(Ramses::instance(), SIGNAL(loggedIn(RamUser*)), this, SLOT(currentUserChanged(RamUser*)));
    connect(ui_resolutionWidget, &ResolutionWidget::resolutionChanged, this, &ProjectEditWidget::update);
    connect(ui_framerateWidget, &FramerateWidget::framerateChanged, this, &ProjectEditWidget::update);
    connect(ui_folderSelector, &DuQFFolderSelectorWidget::pathChanging, this, &ProjectEditWidget::updateFolderLabel);
    connect(ui_folderSelector, &DuQFFolderSelectorWidget::pathChanged, this, &ProjectEditWidget::update);
    connect(ui_deadlineEdit, SIGNAL(dateChanged(QDate)), this, SLOT(update()));
    connect(m_userList, SIGNAL(add()), this, SLOT(createUser()));
    connect(ui_savePathButton, SIGNAL(clicked()), this, SLOT(savePath()));
    connect(ui_reinitPathButton, SIGNAL(clicked()), this, SLOT(reinitPath()));

    monitorDbQuery("updateProject");
}
