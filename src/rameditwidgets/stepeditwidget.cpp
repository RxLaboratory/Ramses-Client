#include "stepeditwidget.h"

StepEditWidget::StepEditWidget(QWidget *parent) : ObjectEditWidget(parent)
{
    setupUi();
    populateMenus();
    connectEvents();

    setObject(nullptr);
}

StepEditWidget::StepEditWidget(RamStep *s, QWidget *parent) : ObjectEditWidget(s, parent)
{
    setupUi();
    populateMenus();
    connectEvents();

    setObject(s);
}

RamStep *StepEditWidget::step() const
{
    return _step;
}

void StepEditWidget::setObject(RamObject *obj)
{
    RamStep *step = qobject_cast<RamStep*>( obj );
    this->setEnabled(false);

    ObjectEditWidget::setObject(step);
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
    for( int i = 0; i < step->users()->count(); i++ ) userAssigned( step->users()->at(i) );

    // Load applications
    foreach (QAction *a , assignAppMenu->actions())
        a->setVisible(true);
    for( int i = 0; i < step->applications()->count(); i++ ) applicationAssigned( step->applications()->at(i) );

    usersList->setList(step->users());
    applicationList->setList(step->applications());

    _objectConnections << connect(step->users(), &RamObjectList::objectAdded, this, &StepEditWidget::userAssigned);
    _objectConnections << connect(step->users(), &RamObjectList::objectRemoved, this, &StepEditWidget::userUnassigned);
    _objectConnections << connect(step->applications(), &RamObjectList::objectAdded, this, &StepEditWidget::applicationAssigned);
    _objectConnections << connect(step->applications(), &RamObjectList::objectRemoved, this, &StepEditWidget::applicationUnassigned);

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

    usersList = new ObjectListEditWidget(true, splitter);
    usersList->setEditMode(RamObjectListWidget::UnassignObjects);
    usersList->setTitle("Users");
    splitter->addWidget(usersList);

    applicationList = new ObjectListEditWidget(true, splitter);
    applicationList->setEditMode(RamObjectListWidget::UnassignObjects);
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
    for(int i = 0; i < Ramses::instance()->applications()->count(); i++) newApplication( Ramses::instance()->applications()->at(i) );
}

void StepEditWidget::connectEvents()
{
    connect(Ramses::instance()->users(), &RamObjectList::objectAdded, this, &StepEditWidget::newUser);
    connect(Ramses::instance()->users(), &RamObjectList::objectRemoved, this, &StepEditWidget::userRemoved);
    connect(Ramses::instance()->applications(), &RamObjectList::objectAdded, this, &StepEditWidget::newApplication);
    connect(Ramses::instance()->applications(), &RamObjectList::objectRemoved, this, &StepEditWidget::applicationRemoved);
    connect(typeBox, SIGNAL(currentIndexChanged(int)), this, SLOT(update()));
}

void StepEditWidget::newUser(RamObject *user)
{
    if (!user) return;
    if (user->uuid() == "") return;
    QAction *userAction = new QAction(user->name());
    userAction->setData(user->uuid());
    assignUserMenu->addAction(userAction);
    QList<QMetaObject::Connection> c;
    c << connect(user, &RamObject::changed, this, &StepEditWidget::userChanged);
    c << connect(user, &RamObject::removed, this, &StepEditWidget::userRemoved);
    m_userConnections[user->uuid()] = c;
    connect(userAction, &QAction::triggered, this, &StepEditWidget::assignUser);
}

void StepEditWidget::assignUser()
{
    if(!_step) return;
    QAction *userAction = (QAction*)sender();
    RamObject *user = Ramses::instance()->users()->fromUuid(  userAction->data().toString() );
    if (!user) return;
    _step->users()->append(user);
}

void StepEditWidget::userAssigned(RamObject *user)
{
    if (!user) return;
    if (user->uuid() == "") return;

    //hide from assign list
    QList<QAction *> actions = assignUserMenu->actions();
    for (int i = 0; i < actions.count(); i++)
    {
        if (actions[i]->data().toString() == user->uuid()) actions[i]->setVisible(false);
    }
}

void StepEditWidget::userUnassigned(RamObject *o)
{
    //show in assign list
    foreach (QAction * a, assignUserMenu->actions() )
        if (a->data().toString() == o->uuid()) a->setVisible(true);
}

void StepEditWidget::userChanged(RamObject *user)
{
    foreach (QAction *a, assignUserMenu->actions())
        if (a->data().toString() == user->uuid()) a->setText(user->name());
}

void StepEditWidget::userRemoved(RamObject *user)
{
    if (m_userConnections.contains(user->uuid()))
    {
        QList<QMetaObject::Connection> c = m_userConnections.value( user->uuid() );
        while (!c.isEmpty()) disconnect( c.takeLast() );
    }

    QList<QAction *> actions = assignUserMenu->actions();
    for( int i = actions.count() -1 ; i >= 0 ; i--)
    {
        if (actions.at(i)->data().toString() == user->uuid()) actions.at(i)->deleteLater();
    }
}

void StepEditWidget::newApplication(RamObject *app)
{
    if (!app) return;
    if (app->uuid() == "") return;
    QAction *appAction = new QAction(app->name());
    appAction->setData(app->uuid());
    assignAppMenu->addAction(appAction);
    QList<QMetaObject::Connection> c;
    c << connect(app, &RamApplication::changed, this, &StepEditWidget::applicationChanged);
    c << connect(app, &RamApplication::removed, this, &StepEditWidget::applicationRemoved);
    m_applicationConnections[app->uuid()] = c;
    connect(appAction, &QAction::triggered, this, &StepEditWidget::assignApplication);
}

void StepEditWidget::assignApplication()
{
    if(!_step) return;
    QAction *appAction = (QAction*)sender();
    RamObject *app = Ramses::instance()->applications()->fromUuid( appAction->data().toString());
    if (!app) return;
    _step->applications()->append(app);
}

void StepEditWidget::applicationAssigned(RamObject *app)
{
    if (!app) return;
    if (app->uuid() == "") return;

    //hide from assign list
    QList<QAction *> actions = assignAppMenu->actions();
    for (int i = 0; i < actions.count(); i++)
    {
        if (actions[i]->data().toString() == app->uuid()) actions[i]->setVisible(false);
    }
}

void StepEditWidget::applicationUnassigned(RamObject *app)
{
    //show in assign list
    foreach (QAction * a, assignAppMenu->actions() )
        if (a->data().toString() == app->uuid()) a->setVisible(true);
}

void StepEditWidget::applicationChanged(RamObject *app)
{
    foreach (QAction *a, assignAppMenu->actions())
        if (a->data().toString() == app->uuid()) a->setText(app->name());
}

void StepEditWidget::applicationRemoved(RamObject *o)
{
    if (m_applicationConnections.contains(o->uuid()))
    {
        QList<QMetaObject::Connection> c = m_applicationConnections.value( o->uuid() );
        while (!c.isEmpty()) disconnect( c.takeLast() );
    }

    QList<QAction *> actions = assignAppMenu->actions();
    for( int i = actions.count() -1 ; i >= 0 ; i--)
    {
        if (actions.at(i)->data().toString() == o->uuid()) actions.at(i)->deleteLater();
    }
}
