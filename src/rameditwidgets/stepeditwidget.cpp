#include "stepeditwidget.h"

StepEditWidget::StepEditWidget(QWidget *parent) : ObjectEditWidget(parent)
{
    setupUi();
    populateMenus();
    connectEvents();

    setStep(nullptr);
}

StepEditWidget::StepEditWidget(RamStep *s, QWidget *parent) : ObjectEditWidget(s, parent)
{
    setupUi();
    populateMenus();
    connectEvents();

    setStep(s);
}

RamStep *StepEditWidget::step() const
{
    return _step;
}

void StepEditWidget::setStep(RamStep *step)
{
    this->setEnabled(false);

    setObject(step);
    _step = step;

    QSignalBlocker b(typeBox);
    QSignalBlocker b1(folderWidget);
    QSignalBlocker b2(usersList);
    QSignalBlocker b3(applicationList);

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
    _objectConnections << connect(step, &RamStep::changed, this, &StepEditWidget::stepChanged);

    this->setEnabled(Ramses::instance()->isProjectAdmin());
}

void StepEditWidget::update()
{
    if(!_step) return;

    updating = true;

    _step->setType(typeBox->currentData().toString());
    ObjectEditWidget::update();

    updating = false;
}

void StepEditWidget::stepChanged(RamObject *o)
{
    if (updating) return;
    Q_UNUSED(o);
    setStep(_step);
}

void StepEditWidget::setupUi()
{
    QLabel *typeLabel = new QLabel("Type", this);
    mainFormLayout->addWidget(typeLabel, 2,0);

    typeBox = new QComboBox(this);
    typeBox->addItem(QIcon(":/icons/project"), "        Pre-Production", "pre");
    typeBox->addItem(QIcon(":/icons/asset"), "        Asset Production", "asset");
    typeBox->addItem(QIcon(":/icons/shot"), "        Shot Production", "shot");
    typeBox->addItem(QIcon(":/icons/film"), "        Post-Production", "post");
    mainFormLayout->addWidget(typeBox, 2, 1);

    folderWidget = new DuQFFolderDisplayWidget(this);
    mainLayout->insertWidget(1, folderWidget);

    QSplitter *splitter = new QSplitter(this);
    splitter->setOrientation(Qt::Vertical);

    usersList = new SimpleObjectList(true, splitter);
    usersList->setTitle("Users");
    splitter->addWidget(usersList);

    applicationList = new SimpleObjectList(true, splitter);
    applicationList->setTitle("Applications");
    splitter->addWidget(applicationList);

    mainLayout->addWidget(splitter);

    // Add menu
    assignUserMenu = new QMenu(this);
    QToolButton *addUserButton = usersList->addButton();
    addUserButton->setPopupMode(QToolButton::InstantPopup);
    addUserButton->setMenu(assignUserMenu);

    assignAppMenu = new QMenu(this);
    QToolButton *addAppButton = applicationList->addButton();
    addAppButton->setPopupMode(QToolButton::InstantPopup);
    addAppButton->setMenu(assignAppMenu);
}

void StepEditWidget::populateMenus()
{
    for (int i = 0; i < Ramses::instance()->users()->count(); i++) newUser( Ramses::instance()->users()->at(i) );
    foreach(RamApplication *a, Ramses::instance()->applications()) newApplication(a);
}

void StepEditWidget::connectEvents()
{
    connect(Ramses::instance()->users(), &RamObjectList::objectAdded, this, &StepEditWidget::newUser);
    connect(Ramses::instance(), &Ramses::newApplication, this, &StepEditWidget::newApplication);
    connect(typeBox, SIGNAL(currentIndexChanged(int)), this, SLOT(update()));
    connect(usersList, &SimpleObjectList::objectRemoved, this, &StepEditWidget::unassignUser);
    connect(applicationList, &SimpleObjectList::objectRemoved, this, &StepEditWidget::unassignApplication);
}

void StepEditWidget::newUser(RamObject *user)
{
    if (!user) return;
    if (user->uuid() == "") return;
    QAction *userAction = new QAction(user->name());
    userAction->setData(user->uuid());
    assignUserMenu->addAction(userAction);
    connect(user, &RamUser::changed, this, &StepEditWidget::userChanged);
    connect(user, &RamUser::removed, this, &StepEditWidget::userRemoved);
    connect(userAction, &QAction::triggered, this, &StepEditWidget::assignUser);
}

void StepEditWidget::assignUser()
{
    if(!_step) return;
    QAction *userAction = (QAction*)sender();
    RamObject *user = Ramses::instance()->users()->fromUuid(  userAction->data().toString() );
    if (!user) return;
    _step->assignUser(user);
}

void StepEditWidget::unassignUser(RamObject *o)
{
    if (!_step) return;
    _step->unassignUser( o );
}

void StepEditWidget::userAssigned(RamUser *user)
{
    if (!user) return;
    if (user->uuid() == "") return;

    usersList->addObject(user);

    //hide from assign list
    QList<QAction *> actions = assignUserMenu->actions();
    for (int i = 0; i < actions.count(); i++)
    {
        if (actions[i]->data().toString() == user->uuid()) actions[i]->setVisible(false);
    }
}

void StepEditWidget::userUnassigned(QString uuid)
{
    usersList->removeObject(uuid);

    //show in assign list
    foreach (QAction * a, assignUserMenu->actions() )
        if (a->data().toString() == uuid) a->setVisible(true);
}

void StepEditWidget::userChanged()
{
    RamUser *user = (RamUser*)sender();

    foreach (QAction *a, assignUserMenu->actions())
        if (a->data().toString() == user->uuid()) a->setText(user->name());
}

void StepEditWidget::userRemoved(RamObject *user)
{
    foreach (QAction *a, assignUserMenu->actions())
        if (a->data().toString() == user->uuid())
            a->deleteLater();
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

void StepEditWidget::unassignApplication(RamObject *o)
{
    if (!_step) return;
    _step->unassignApplication( o );
}

void StepEditWidget::applicationAssigned(RamApplication *app)
{
    if (!app) return;
    if (app->uuid() == "") return;

    applicationList->addObject(app);

    //hide from assign list
    foreach (QAction *a, assignAppMenu->actions())
        if (a->data().toString() == app->uuid()) a->setVisible(false);
}

void StepEditWidget::applicationUnassigned(QString uuid)
{
    applicationList->removeObject(uuid);

    //show in assign list
    foreach (QAction * a, assignAppMenu->actions() )
        if (a->data().toString() == uuid) a->setVisible(true);
}

void StepEditWidget::applicationChanged()
{
    RamApplication *app = (RamApplication*)sender();

    foreach (QAction *a, assignAppMenu->actions())
        if (a->data().toString() == app->uuid()) a->setText(app->name());
}

void StepEditWidget::applicationRemoved(RamObject *o)
{
    foreach (QAction *a, assignAppMenu->actions())
        if (a->data().toString() == o->uuid())
            a->deleteLater();
}
