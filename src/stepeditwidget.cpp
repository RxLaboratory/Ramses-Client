#include "stepeditwidget.h"

StepEditWidget::StepEditWidget(QWidget *parent) : ObjectEditWidget(parent)
{
    setupUi();
    populateMenus();
    connectEvents();
}

StepEditWidget::StepEditWidget(RamStep *s, QWidget *parent) : ObjectEditWidget(parent)
{
    setupUi();
    populateMenus();
    connectEvents();

    setStep(s);
}

void StepEditWidget::setStep(RamStep *step)
{
    this->setEnabled(false);

    setObject(step);

    typeBox->setCurrentIndex(1);
    folderWidget->setPath("");
    usersList->clear();
    applicationList->clear();

    if (!step) return;

    folderWidget->setPath(Ramses::instance()->path(step));
    if (step->type() == RamStep::PreProduction) typeBox->setCurrentIndex(0);
    else if (step->type() == RamStep::AssetProduction) typeBox->setCurrentIndex(1);
    else if (step->type() == RamStep::ShotProduction) typeBox->setCurrentIndex(2);
    else if (step->type() == RamStep::PostProduction) typeBox->setCurrentIndex(3);

    // Load Users
    foreach (QAction *a , assignUserMenu->actions())
        a->setVisible(true);
    foreach( RamUser *user, step->users()) userAssigned(user);

    // Load applications
    foreach (QAction *a , assignAppMenu->actions())
        a->setVisible(true);
    foreach( RamApplication *app, step->applications()) applicationAssigned(app);

    _objectConnections << connect(step, &RamStep::userAssigned, this, &StepEditWidget::userAssigned);
    _objectConnections << connect(step, &RamStep::userUnassigned, this, &StepEditWidget::userUnassigned);
    _objectConnections << connect(step, &RamStep::applicationAssigned, this, &StepEditWidget::applicationAssigned);
    _objectConnections << connect(step, &RamStep::applicationUnassigned, this, &StepEditWidget::applicationUnassigned);

    this->setEnabled(Ramses::instance()->isProjectAdmin());
}

void StepEditWidget::update()
{
    RamStep *step = qobject_cast<RamStep*>(object());
    if(!step) return;

    step->setType(typeBox->currentData().toString());

    ObjectEditWidget::update();
}

void StepEditWidget::setupUi()
{
    QLabel *typeLabel = new QLabel("Type", this);
    mainFormLayout->addWidget(typeLabel, 2,0);

    typeBox = new QComboBox(this);
    typeBox->addItem(QIcon(":/icons/project"), "Pre-Production", "pre");
    typeBox->addItem(QIcon(":/icons/asset"), "Asset Production", "asset");
    typeBox->addItem(QIcon(":/icons/shot"), "Shot Production", "shot");
    typeBox->addItem(QIcon(":/icons/film"), "Post-Production", "post");
    mainFormLayout->addWidget(typeBox, 2, 1);

    folderWidget = new DuQFFolderDisplayWidget(this);
    mainFormLayout->addWidget(folderWidget, 3, 1);

    QSplitter *splitter = new QSplitter(this);
    splitter->setOrientation(Qt::Vertical);

    QWidget *usersWidget = new QWidget(splitter);
    QHBoxLayout *usersLayout = new QHBoxLayout(usersWidget);
    usersLayout->setSpacing(3);
    usersLayout->setContentsMargins(0, 0, 0, 0);
    QLabel *usersLabel = new QLabel("Users", usersWidget);
    usersLabel->setMinimumWidth(50);
    usersLabel->setMaximumWidth(50);
    usersLabel->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
    usersLayout->addWidget(usersLabel);

    QVBoxLayout *usersButtonsLayout = new QVBoxLayout();
    usersButtonsLayout->setSpacing(3);
    usersButtonsLayout->setContentsMargins(3, 0, 0, 0);

    assignUserButton = new QToolButton(usersWidget);
    assignUserButton->setIcon(QIcon(":/icons/add"));
    assignUserButton->setPopupMode(QToolButton::InstantPopup);
    usersButtonsLayout->addWidget(assignUserButton);

    removeUserButton = new QToolButton(usersWidget);
    removeUserButton->setIcon(QIcon(":/icons/remove"));
    removeUserButton->setPopupMode(QToolButton::InstantPopup);
    usersButtonsLayout->addWidget(removeUserButton);

    usersButtonsLayout->addStretch();

    usersLayout->addLayout(usersButtonsLayout);

    usersList = new QListWidget(usersWidget);
    usersList->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    usersLayout->addWidget(usersList);

    splitter->addWidget(usersWidget);

    QWidget *applicationsWidget = new QWidget(splitter);
    QHBoxLayout *applicationsLayout = new QHBoxLayout(applicationsWidget);
    applicationsLayout->setSpacing(3);
    applicationsLayout->setContentsMargins(0, 0, 0, 0);
    QLabel *applicationsLabel = new QLabel("Apps", applicationsWidget);
    applicationsLabel->setMinimumWidth(50);
    applicationsLabel->setMaximumWidth(50);
    applicationsLabel->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
    applicationsLayout->addWidget(applicationsLabel);

    QVBoxLayout *applicationsButtonsLayout = new QVBoxLayout();
    applicationsButtonsLayout->setSpacing(3);
    applicationsButtonsLayout->setContentsMargins(3, 0, 0, 0);

    assignApplicationButton = new QToolButton(applicationsWidget);
    assignApplicationButton->setIcon(QIcon(":/icons/add"));
    assignApplicationButton->setPopupMode(QToolButton::InstantPopup);
    applicationsButtonsLayout->addWidget(assignApplicationButton);

    removeApplicationButton = new QToolButton(applicationsWidget);
    removeApplicationButton->setIcon(QIcon(":/icons/remove"));
    removeApplicationButton->setPopupMode(QToolButton::InstantPopup);
    applicationsButtonsLayout->addWidget(removeApplicationButton);

    applicationsButtonsLayout->addStretch();

    applicationsLayout->addLayout(applicationsButtonsLayout);

    applicationList = new QListWidget(applicationsWidget);
    applicationList->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    applicationsLayout->addWidget(applicationList);

    splitter->addWidget(applicationsWidget);

    mainLayout->insertWidget(2, splitter);

    // Add menu
    assignUserMenu = new QMenu(this);
    assignUserButton->setMenu(assignUserMenu);

    assignAppMenu = new QMenu(this);
    assignApplicationButton->setMenu(assignAppMenu);
}

void StepEditWidget::populateMenus()
{
    foreach(RamUser *u, Ramses::instance()->users()) newUser(u);
    foreach(RamApplication *a, Ramses::instance()->applications()) newApplication(a);
}

void StepEditWidget::connectEvents()
{
    connect(Ramses::instance(), &Ramses::newUser, this, &StepEditWidget::newUser);
    connect(Ramses::instance(), &Ramses::newApplication, this, &StepEditWidget::newApplication);
    connect(removeUserButton, &QToolButton::clicked, this, &StepEditWidget::unassignUser);
    connect(removeApplicationButton, &QToolButton::clicked, this, &StepEditWidget::unassignApplication);
}

void StepEditWidget::newUser(RamUser *user)
{
    if (!user) return;
    if (user->uuid() == "") return;
    QAction *userAction = new QAction(user->name());
    userAction->setData(user->uuid());
    assignUserMenu->addAction(userAction);
    connect(user, &RamUser::changed, this, &StepEditWidget::userChanged);
    connect(user, &RamUser::destroyed, this, &StepEditWidget::userDestroyed);
    connect(userAction, &QAction::triggered, this, &StepEditWidget::assignUser);
}

void StepEditWidget::assignUser()
{
    RamStep *step = qobject_cast<RamStep*>(object());

    if(!step) return;
    QAction *userAction = (QAction*)sender();
    RamUser *user = Ramses::instance()->user( userAction->data().toString());
    if (!user) return;
    step->assignUser(user);
}

void StepEditWidget::unassignUser()
{
    RamStep *step = qobject_cast<RamStep*>(object());

    if (!step) return;
    step->unassignUser( usersList->currentItem()->data(Qt::UserRole).toString() );
}

void StepEditWidget::userAssigned(RamUser *user)
{
    if (!user) return;
    if (user->uuid() == "") return;

    QListWidgetItem *userItem = new QListWidgetItem(user->name());
    userItem->setData(Qt::UserRole, user->uuid());
    usersList->addItem(userItem);
    connect(user, &RamUser::destroyed, this, &StepEditWidget::userDestroyed);
    connect(user, &RamUser::changed, this, &StepEditWidget::userChanged);

    //hide from assign list
    QList<QAction *> actions = assignUserMenu->actions();
    for (int i = 0; i < actions.count(); i++)
    {
        if (actions[i]->data().toString() == user->uuid()) actions[i]->setVisible(false);
    }
}

void StepEditWidget::userUnassigned(QString uuid)
{
    for (int i = usersList->count() - 1; i >= 0; i--)
        if (usersList->item(i)->data(Qt::UserRole).toString() == uuid)
            delete usersList->takeItem(i);

    //show in assign list
    foreach (QAction * a, assignUserMenu->actions() )
        if (a->data().toString() == uuid) a->setVisible(true);
}

void StepEditWidget::userChanged()
{
    RamUser *user = (RamUser*)sender();

    foreach (QAction *a, assignUserMenu->actions())
        if (a->data().toString() == user->uuid()) a->setText(user->name());

    for (int i = usersList->count() -1; i >= 0; i--)
        if (usersList->item(i)->data(Qt::UserRole).toString() == user->uuid())
            usersList->item(i)->setText(user->name());
}

void StepEditWidget::userDestroyed(QObject *o)
{
    RamUser *u = (RamUser *)o;

    foreach (QAction *a, assignUserMenu->actions())
        if (a->data().toString() == u->uuid())
            a->deleteLater();

    for (int i = usersList->count() -1; i >= 0; i--)
        if (usersList->item(i)->data(Qt::UserRole).toString() == u->uuid())
            delete usersList->takeItem(i);
}

void StepEditWidget::newApplication(RamApplication *app)
{
    if (!app) return;
    if (app->uuid() == "") return;
    QAction *appAction = new QAction(app->name());
    appAction->setData(app->uuid());
    assignAppMenu->addAction(appAction);
    connect(app, &RamApplication::changed, this, &StepEditWidget::applicationChanged);
    connect(app, &RamApplication::removed, this, &StepEditWidget::applicationRemoved);
    connect(appAction, &QAction::triggered, this, &StepEditWidget::assignApplication);
}

void StepEditWidget::assignApplication()
{
    RamStep *step = qobject_cast<RamStep*>(object());

    if(!step) return;
    QAction *appAction = (QAction*)sender();
    RamApplication *app = Ramses::instance()->application( appAction->data().toString());
    if (!app) return;
    step->assignApplication(app);
}

void StepEditWidget::unassignApplication()
{
    RamStep *step = qobject_cast<RamStep*>(object());

    if (!step) return;
    step->unassignApplication( applicationList->currentItem()->data(Qt::UserRole).toString() );
}

void StepEditWidget::applicationAssigned(RamApplication *app)
{
    if (!app) return;
    if (app->uuid() == "") return;

    QListWidgetItem *appItem = new QListWidgetItem(app->name());
    appItem->setData(Qt::UserRole, app->uuid());
    applicationList->addItem(appItem);
    connect(app, &RamApplication::removed, this, &StepEditWidget::applicationRemoved);
    connect(app, &RamApplication::changed, this, &StepEditWidget::applicationChanged);

    //hide from assign list
    foreach (QAction *a, assignAppMenu->actions())
        if (a->data().toString() == app->uuid()) a->setVisible(false);
}

void StepEditWidget::applicationUnassigned(QString uuid)
{
    for (int i = applicationList->count() - 1; i >= 0; i--)
        if (applicationList->item(i)->data(Qt::UserRole).toString() == uuid)
            delete applicationList->takeItem(i);

    //show in assign list
    foreach (QAction * a, assignAppMenu->actions() )
        if (a->data().toString() == uuid) a->setVisible(true);
}

void StepEditWidget::applicationChanged()
{
    RamApplication *app = (RamApplication*)sender();

    foreach (QAction *a, assignAppMenu->actions())
        if (a->data().toString() == app->uuid()) a->setText(app->name());

    for (int i = applicationList->count() -1; i >= 0; i--)
        if (applicationList->item(i)->data(Qt::UserRole).toString() == app->uuid())
            applicationList->item(i)->setText(app->name());
}

void StepEditWidget::applicationRemoved(RamObject *o)
{
    foreach (QAction *a, assignAppMenu->actions())
        if (a->data().toString() == o->uuid())
            a->deleteLater();

    for (int i = applicationList->count() -1; i >= 0; i--)
        if (applicationList->item(i)->data(Qt::UserRole).toString() == o->uuid())
            delete applicationList->takeItem(i);
}
