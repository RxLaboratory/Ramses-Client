#include "stepeditwidget.h"

StepEditWidget::StepEditWidget(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);

    typeBox->setItemData(0, "pre");
    typeBox->setItemData(1, "asset");
    typeBox->setItemData(2, "shot");
    typeBox->setItemData(3, "post");

    // Add menu
    assignMenu = new QMenu(this);
    assignUserButton->setMenu(assignMenu);

    connect(updateButton, SIGNAL(clicked()), this, SLOT(update()));
    connect(revertButton, SIGNAL(clicked()), this, SLOT(revert()));
    connect(shortNameEdit, &QLineEdit::textChanged, this, &StepEditWidget::checkInput);
    connect(DBInterface::instance(),&DBInterface::log, this, &StepEditWidget::dbiLog);
    connect(Ramses::instance(), &Ramses::newUser, this, &StepEditWidget::newUser);
    connect(removeUserButton, &QToolButton::clicked, this, &StepEditWidget::removeUser);

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
    this->setEnabled(false);

    if (!step) return;

    nameEdit->setText(step->name());
    shortNameEdit->setText(step->shortName());

    if (step->type() == RamStep::PreProduction) typeBox->setCurrentIndex(0);
    else if (step->type() == RamStep::AssetProduction) typeBox->setCurrentIndex(1);
    else if (step->type() == RamStep::ShotProduction) typeBox->setCurrentIndex(2);
    else if (step->type() == RamStep::PostProduction) typeBox->setCurrentIndex(3);

    // Load Users
    usersList->clear();
    // Reset assign list too
    QList<QAction *> actions = assignMenu->actions();
    for (int i = 0; i < actions.count(); i++)
    {
        actions[i]->setVisible(true);
    }
    foreach( RamUser *user, step->users()) userAssigned(user);

    _stepConnections << connect(step, &RamStep::newUser, this, &StepEditWidget::userAssigned);
    _stepConnections << connect(step, &RamStep::userRemoved, this, &StepEditWidget::userRemoved);
    _stepConnections << connect(step, &RamStep::destroyed, this, &StepEditWidget::stepDestroyed);

    this->setEnabled(Ramses::instance()->isAdmin());
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
    assignMenu->addAction(userAction);
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

void StepEditWidget::removeUser()
{
    if (!_step) return;
    _step->removeUser( usersList->currentItem()->data(Qt::UserRole).toString() );
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
    QList<QAction *> actions = assignMenu->actions();
    for (int i = 0; i < actions.count(); i++)
    {
        if (actions[i]->data().toString() == user->uuid()) actions[i]->setVisible(false);
    }
}

void StepEditWidget::userRemoved(QString uuid)
{
    for (int i = usersList->count() - 1; i >= 0; i--)
    {
        if (usersList->item(i)->data(Qt::UserRole).toString() == uuid)
        {
            QListWidgetItem *item = usersList->takeItem(i);
            delete item;
        }
    }

    //hide from assign list
    QList<QAction *> actions = assignMenu->actions();
    for (int i = 0; i < actions.count(); i++)
    {
        if (actions[i]->data().toString() == uuid) actions[i]->setVisible(true);
    }
}

void StepEditWidget::userChanged()
{
    RamUser *user = (RamUser*)sender();
    QList<QAction *> actions = assignMenu->actions();
    for (int i = actions.count() -1; i >= 0; i--)
    {
        if (actions[i]->data().toString() == user->uuid()) actions[i]->setText(user->name());
    }

    for (int i = usersList->count() -1; i >= 0; i--)
    {
        if (usersList->item(i)->data(Qt::UserRole).toString() == user->uuid())
        {
            usersList->item(i)->setText(user->name());
        }
    }
}

void StepEditWidget::userDestroyed(QObject *o)
{
    RamUser *u = (RamUser *)o;
    QList<QAction *> actions = assignMenu->actions();
    for (int i = actions.count() -1; i >= 0; i--)
    {
        if (actions[i]->data().toString() == u->uuid())
        {
            assignMenu->removeAction(actions[i]);
            actions[i]->deleteLater();
        }
    }

    for (int i = usersList->count() -1; i >= 0; i--)
    {
        if (usersList->item(i)->data(Qt::UserRole).toString() == u->uuid())
        {
            QListWidgetItem *item = usersList->takeItem(i);
            delete item;
        }
    }
}

void StepEditWidget::dbiLog(QString m, LogUtils::LogType t)
{
    if (t != LogUtils::Remote && t != LogUtils::Debug) statusLabel->setText(m);
}
