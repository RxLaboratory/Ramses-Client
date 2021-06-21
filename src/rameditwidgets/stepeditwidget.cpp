#include "stepeditwidget.h"

StepEditWidget::StepEditWidget(QWidget *parent) : ObjectEditWidget(parent)
{
    setupUi();
    connectEvents();

    setObject(nullptr);
}

StepEditWidget::StepEditWidget(RamStep *s, QWidget *parent) : ObjectEditWidget(s, parent)
{
    setupUi();
    connectEvents();

    setObject(s);
}

RamStep *StepEditWidget::step() const
{
    return m_step;
}

void StepEditWidget::setObject(RamObject *obj)
{
    RamStep *step = qobject_cast<RamStep*>( obj );
    this->setEnabled(false);

    ObjectEditWidget::setObject(step);
    m_step = step;

    QSignalBlocker b(m_typeBox);
    QSignalBlocker b1(m_folderWidget);
    QSignalBlocker b2(m_userList);
    QSignalBlocker b3(m_applicationList);

    m_typeBox->setCurrentIndex(1);
    m_folderWidget->setPath("");
    m_userList->clear();
    m_applicationList->clear();

    if (!step) return;

    m_folderWidget->setPath(Ramses::instance()->path(step));
    if (step->type() == RamStep::PreProduction) m_typeBox->setCurrentIndex(0);
    else if (step->type() == RamStep::AssetProduction) m_typeBox->setCurrentIndex(1);
    else if (step->type() == RamStep::ShotProduction) m_typeBox->setCurrentIndex(2);
    else if (step->type() == RamStep::PostProduction) m_typeBox->setCurrentIndex(3);

    m_userList->setList(step->users());
    m_applicationList->setList(step->applications());

    this->setEnabled(Ramses::instance()->isProjectAdmin());
}

void StepEditWidget::update()
{
    if(!m_step) return;

    updating = true;

    m_step->setType(m_typeBox->currentData().toString());
    ObjectEditWidget::update();

    updating = false;
}

void StepEditWidget::createUser()
{
    if (!m_step) return;
    RamUser *user = new RamUser(
                "NEW",
                "John Doe");
    Ramses::instance()->users()->append(user);
    m_step->users()->append(user);
    user->edit();
}

void StepEditWidget::createApplication()
{
    if (!m_step) return;
    RamApplication *app = new RamApplication(
                "NEW",
                "New Application");
    Ramses::instance()->applications()->append(app);
    m_step->applications()->append(app);
    app->edit();
}

void StepEditWidget::setupUi()
{
    QLabel *typeLabel = new QLabel("Type", this);
    mainFormLayout->addWidget(typeLabel, 2,0);

    m_typeBox = new QComboBox(this);
    m_typeBox->addItem(QIcon(":/icons/project"), "        Pre-Production", "pre");
    m_typeBox->addItem(QIcon(":/icons/asset"), "        Asset Production", "asset");
    m_typeBox->addItem(QIcon(":/icons/shot"), "        Shot Production", "shot");
    m_typeBox->addItem(QIcon(":/icons/film"), "        Post-Production", "post");
    mainFormLayout->addWidget(m_typeBox, 2, 1);

    m_folderWidget = new DuQFFolderDisplayWidget(this);
    mainLayout->insertWidget(1, m_folderWidget);

    QSplitter *splitter = new QSplitter(this);
    splitter->setOrientation(Qt::Vertical);

    m_userList = new ObjectListEditWidget(true, RamUser::ProjectAdmin, splitter);
    m_userList->setEditMode(ObjectListEditWidget::UnassignObjects);
    m_userList->setTitle("Users");
    m_userList->setAssignList(Ramses::instance()->users());
    splitter->addWidget(m_userList);

    m_applicationList = new ObjectListEditWidget(true, RamUser::ProjectAdmin, splitter);
    m_applicationList->setEditMode(ObjectListEditWidget::UnassignObjects);
    m_applicationList->setTitle("Applications");
    m_applicationList->setAssignList(Ramses::instance()->applications());
    splitter->addWidget(m_applicationList);

    mainLayout->addWidget(splitter);
}

void StepEditWidget::connectEvents()
{
    connect(m_userList, SIGNAL(add()), this, SLOT(createUser()));
    connect(m_applicationList, SIGNAL(add()), this, SLOT(createApplication()));
}
