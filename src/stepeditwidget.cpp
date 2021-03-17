#include "stepeditwidget.h"

StepEditWidget::StepEditWidget(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);

    folderWidget = new DuQFFolderDisplayWidget(this);
    folderLayout->addWidget(folderWidget);

    typeBox->setItemData(0, "pre");
    typeBox->setItemData(1, "asset");
    typeBox->setItemData(2, "shot");
    typeBox->setItemData(3, "post");

    // Add menu
    assignUserMenu = new QMenu(this);
    assignUserButton->setMenu(assignUserMenu);

    assignAppMenu = new QMenu(this);
    assignApplicationButton->setMenu(assignAppMenu);

    connect(updateButton, SIGNAL(clicked()), this, SLOT(update()));
    connect(revertButton, SIGNAL(clicked()), this, SLOT(revert()));
    connect(shortNameEdit, &QLineEdit::textChanged, this, &StepEditWidget::checkInput);
    connect(DBInterface::instance(),&DBInterface::newLog, this, &StepEditWidget::dbiLog);
    connect(Ramses::instance(), &Ramses::newUser, this, &StepEditWidget::newUser);
    connect(Ramses::instance(), &Ramses::newApplication, this, &StepEditWidget::newApplication);
    connect(removeUserButton, &QToolButton::clicked, this, &StepEditWidget::unassignUser);
    connect(removeApplicationButton, &QToolButton::clicked, this, &StepEditWidget::unassignApplication);

    this->setEnabled(false);
}

RamStep *StepEditWidget::step() const
{
    return _step;
}

void StepEditWidget::setStep(RamStep *step)
{
    while (_stepConnections.count() > 0) disconnect( _stepConnections.takeLast() );

    _step = step;

    nameEdit->setText("");
    shortNameEdit->setText("");
    typeBox->setCurrentIndex(1);
    folderWidget->setPath("");
    usersList->clear();
    applicationList->clear();
    this->setEnabled(false);

    if (!step) return;

    nameEdit->setText(step->name());
    shortNameEdit->setText(step->shortName());
    folderWidget->setPath(Ramses::instance()->path(step));

    if (step->type() == RamStep::PreProduction) typeBox->setCurrentIndex(0);
    else if (step->type() == RamStep::AssetProduction) typeBox->setCurrentIndex(1);
    else if (step->type() == RamStep::ShotProduction) typeBox->setCurrentIndex(2);
    else if (step->type() == RamStep::PostProduction) typeBox->setCurrentIndex(3);

    // Load Users

    // Reset assign list too
    foreach (QAction *a , assignUserMenu->actions())
        a->setVisible(true);
    foreach( RamUser *user, step->users()) userAssigned(user);

    // Load applications
    foreach (QAction *a , assignAppMenu->actions())
        a->setVisible(true);
    foreach( RamApplication *app, step->applications()) applicationAssigned(app);


    _stepConnections << connect(step, &RamStep::userAssigned, this, &StepEditWidget::userAssigned);
    _stepConnections << connect(step, &RamStep::userUnassigned, this, &StepEditWidget::userUnassigned);
    _stepConnections << connect(step, &RamStep::applicationAssigned, this, &StepEditWidget::applicationAssigned);
    _stepConnections << connect(step, &RamStep::applicationUnassigned, this, &StepEditWidget::applicationUnassigned);
    _stepConnections << connect(step, &RamStep::destroyed, this, &StepEditWidget::stepDestroyed);

    this->setEnabled(Ramses::instance()->isProjectAdmin());
}

void StepEditWidget::update()
{
    if (!_step) return;

    this->setEnabled(false);

    //check if everything is alright
    if (!checkInput())
    {
        this->setEnabled(true);
        return;
    }

    _step->setName(nameEdit->text());
    _step->setShortName(shortNameEdit->text());
    _step->setType(typeBox->currentData().toString());

    _step->update();

    this->setEnabled(true);
}

void StepEditWidget::revert()
{
    setStep(_step);
}

bool StepEditWidget::checkInput()
{
    if (!_step) return false;

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

void StepEditWidget::stepDestroyed(QObject */*o*/)
{
    setStep(nullptr);
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
    if(!_step) return;
    QAction *userAction = (QAction*)sender();
    RamUser *user = Ramses::instance()->user( userAction->data().toString());
    if (!user) return;
    _step->assignUser(user);
}

void StepEditWidget::unassignUser()
{
    if (!_step) return;
    _step->unassignUser( usersList->currentItem()->data(Qt::UserRole).toString() );
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
    if(!_step) return;
    QAction *appAction = (QAction*)sender();
    RamApplication *app = Ramses::instance()->application( appAction->data().toString());
    if (!app) return;
    _step->assignApplication(app);
}

void StepEditWidget::unassignApplication()
{
    if (!_step) return;
    _step->unassignApplication( applicationList->currentItem()->data(Qt::UserRole).toString() );
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

void StepEditWidget::dbiLog(DuQFLog m)
{
    if (m.type() != DuQFLog::Debug) statusLabel->setText(m.message());
}
