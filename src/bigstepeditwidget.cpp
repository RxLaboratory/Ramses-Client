#include "bigstepeditwidget.h"

BigStepEditWidget::BigStepEditWidget(QWidget *parent) :
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
    connect(shortNameEdit, &QLineEdit::textChanged, this, &BigStepEditWidget::checkInput);
    connect(DBInterface::instance(),&DBInterface::newLog, this, &BigStepEditWidget::dbiLog);
    connect(Ramses::instance(), &Ramses::newUser, this, &BigStepEditWidget::newUser);
    connect(Ramses::instance(), &Ramses::newApplication, this, &BigStepEditWidget::newApplication);
    connect(removeUserButton, &QToolButton::clicked, this, &BigStepEditWidget::unassignUser);
    connect(removeApplicationButton, &QToolButton::clicked, this, &BigStepEditWidget::unassignApplication);

    this->setEnabled(false);
}

RamStep *BigStepEditWidget::step() const
{
    return _step;
}

void BigStepEditWidget::setStep(RamStep *step)
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


    _stepConnections << connect(step, &RamStep::userAssigned, this, &BigStepEditWidget::userAssigned);
    _stepConnections << connect(step, &RamStep::userUnassigned, this, &BigStepEditWidget::userUnassigned);
    _stepConnections << connect(step, &RamStep::applicationAssigned, this, &BigStepEditWidget::applicationAssigned);
    _stepConnections << connect(step, &RamStep::applicationUnassigned, this, &BigStepEditWidget::applicationUnassigned);
    _stepConnections << connect(step, &RamStep::removed, this, &BigStepEditWidget::stepRemoved);

    this->setEnabled(Ramses::instance()->isProjectAdmin());
}

void BigStepEditWidget::update()
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

void BigStepEditWidget::revert()
{
    setStep(_step);
}

bool BigStepEditWidget::checkInput()
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

void BigStepEditWidget::stepRemoved(RamObject */*o*/)
{
    setStep(nullptr);
}

void BigStepEditWidget::newUser(RamUser *user)
{
    if (!user) return;
    if (user->uuid() == "") return;
    QAction *userAction = new QAction(user->name());
    userAction->setData(user->uuid());
    assignUserMenu->addAction(userAction);
    connect(user, &RamUser::changed, this, &BigStepEditWidget::userChanged);
    connect(user, &RamUser::removed, this, &BigStepEditWidget::userRemoved);
    connect(userAction, &QAction::triggered, this, &BigStepEditWidget::assignUser);
}

void BigStepEditWidget::assignUser()
{
    if(!_step) return;
    QAction *userAction = (QAction*)sender();
    RamUser *user = Ramses::instance()->user( userAction->data().toString());
    if (!user) return;
    _step->assignUser(user);
}

void BigStepEditWidget::unassignUser()
{
    if (!_step) return;
    _step->unassignUser( usersList->currentItem()->data(Qt::UserRole).toString() );
}

void BigStepEditWidget::userAssigned(RamUser *user)
{
    if (!user) return;
    if (user->uuid() == "") return;

    QListWidgetItem *userItem = new QListWidgetItem(user->name());
    userItem->setData(Qt::UserRole, user->uuid());
    usersList->addItem(userItem);
    connect(user, &RamUser::removed, this, &BigStepEditWidget::userRemoved);
    connect(user, &RamUser::changed, this, &BigStepEditWidget::userChanged);

    //hide from assign list
    QList<QAction *> actions = assignUserMenu->actions();
    for (int i = 0; i < actions.count(); i++)
    {
        if (actions[i]->data().toString() == user->uuid()) actions[i]->setVisible(false);
    }
}

void BigStepEditWidget::userUnassigned(QString uuid)
{
    for (int i = usersList->count() - 1; i >= 0; i--)
        if (usersList->item(i)->data(Qt::UserRole).toString() == uuid)
            delete usersList->takeItem(i);

    //show in assign list
    foreach (QAction * a, assignUserMenu->actions() )
        if (a->data().toString() == uuid) a->setVisible(true);
}

void BigStepEditWidget::userChanged()
{
    RamUser *user = (RamUser*)sender();

    foreach (QAction *a, assignUserMenu->actions())
        if (a->data().toString() == user->uuid()) a->setText(user->name());

    for (int i = usersList->count() -1; i >= 0; i--)
        if (usersList->item(i)->data(Qt::UserRole).toString() == user->uuid())
            usersList->item(i)->setText(user->name());
}

void BigStepEditWidget::userRemoved(RamObject *o)
{
    foreach (QAction *a, assignUserMenu->actions())
        if (a->data().toString() == o->uuid())
            a->deleteLater();

    for (int i = usersList->count() -1; i >= 0; i--)
        if (usersList->item(i)->data(Qt::UserRole).toString() == o->uuid())
            delete usersList->takeItem(i);
}

void BigStepEditWidget::newApplication(RamApplication *app)
{
    if (!app) return;
    if (app->uuid() == "") return;
    QAction *appAction = new QAction(app->name());
    appAction->setData(app->uuid());
    assignAppMenu->addAction(appAction);
    connect(app, &RamApplication::changed, this, &BigStepEditWidget::applicationChanged);
    connect(app, &RamApplication::removed, this, &BigStepEditWidget::applicationRemoved);
    connect(appAction, &QAction::triggered, this, &BigStepEditWidget::assignApplication);
}

void BigStepEditWidget::assignApplication()
{
    if(!_step) return;
    QAction *appAction = (QAction*)sender();
    RamApplication *app = Ramses::instance()->application( appAction->data().toString());
    if (!app) return;
    _step->assignApplication(app);
}

void BigStepEditWidget::unassignApplication()
{
    if (!_step) return;
    _step->unassignApplication( applicationList->currentItem()->data(Qt::UserRole).toString() );
}

void BigStepEditWidget::applicationAssigned(RamApplication *app)
{
    if (!app) return;
    if (app->uuid() == "") return;

    QListWidgetItem *appItem = new QListWidgetItem(app->name());
    appItem->setData(Qt::UserRole, app->uuid());
    applicationList->addItem(appItem);
    connect(app, &RamApplication::removed, this, &BigStepEditWidget::applicationRemoved);
    connect(app, &RamApplication::changed, this, &BigStepEditWidget::applicationChanged);

    //hide from assign list
    foreach (QAction *a, assignAppMenu->actions())
        if (a->data().toString() == app->uuid()) a->setVisible(false);
}

void BigStepEditWidget::applicationUnassigned(QString uuid)
{
    for (int i = applicationList->count() - 1; i >= 0; i--)
        if (applicationList->item(i)->data(Qt::UserRole).toString() == uuid)
            delete applicationList->takeItem(i);

    //show in assign list
    foreach (QAction * a, assignAppMenu->actions() )
        if (a->data().toString() == uuid) a->setVisible(true);
}

void BigStepEditWidget::applicationChanged()
{
    RamApplication *app = (RamApplication*)sender();

    foreach (QAction *a, assignAppMenu->actions())
        if (a->data().toString() == app->uuid()) a->setText(app->name());

    for (int i = applicationList->count() -1; i >= 0; i--)
        if (applicationList->item(i)->data(Qt::UserRole).toString() == app->uuid())
            applicationList->item(i)->setText(app->name());
}

void BigStepEditWidget::applicationRemoved(RamObject *o)
{
    foreach (QAction *a, assignAppMenu->actions())
        if (a->data().toString() == o->uuid())
            a->deleteLater();

    for (int i = applicationList->count() -1; i >= 0; i--)
        if (applicationList->item(i)->data(Qt::UserRole).toString() == o->uuid())
            delete applicationList->takeItem(i);
}

void BigStepEditWidget::dbiLog(DuQFLog m)
{
    if (m.type() != DuQFLog::Debug) statusLabel->setText(m.message());
}
