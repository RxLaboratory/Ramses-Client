#include "applicationeditwidget.h"

#include "objectlisteditwidget.h"

ApplicationEditWidget::ApplicationEditWidget(QWidget *parent) : ObjectEditWidget(parent)
{
    setupUi();
    setObject(nullptr);
}

ApplicationEditWidget::ApplicationEditWidget(RamApplication *app, QWidget *parent) : ObjectEditWidget(app, parent)
{
    setupUi();
    setObject(app);
}

RamApplication *ApplicationEditWidget::application() const
{
    return m_application;
}

void ApplicationEditWidget::setObject(RamObject *obj)
{
    RamApplication *application = qobject_cast<RamApplication*>( obj );
    this->setEnabled(false);

    ObjectEditWidget::setObject(application);
    m_application = application;

    QSignalBlocker b(m_folderSelector);
    QSignalBlocker b1(m_nativeList);
    QSignalBlocker b2(m_importList);
    QSignalBlocker b3(m_exportList);

    m_folderSelector->setPath("");
    m_nativeList->clear();
    m_importList->clear();
    m_exportList->clear();

    if (!application) return;

    m_folderSelector->setPath(application->executableFilePath());

    m_nativeList->setList(application->nativeFileTypes());
    m_importList->setList(application->importFileTypes());
    m_exportList->setList(application->exportFileTypes());

    this->setEnabled(Ramses::instance()->isProjectAdmin());
}

void ApplicationEditWidget::update()
{
    if (!m_application) return;

    updating = true;

    m_application->setExecutableFilePath(m_folderSelector->path());
    ObjectEditWidget::update();

    updating = false;
}

void ApplicationEditWidget::setupUi()
{
    QLabel *fileLabel = new QLabel("Executable file", this);
    mainFormLayout->addWidget(fileLabel, 2, 0);

    m_folderSelector = new DuQFFolderSelectorWidget(DuQFFolderSelectorWidget::File, this);
    mainFormLayout->addWidget(m_folderSelector, 2, 1);

    QSplitter *splitter = new QSplitter(this);
    splitter->setOrientation(Qt::Vertical);

    m_nativeList = new ObjectListEditWidget(true, RamUser::Admin, splitter);
    m_nativeList->setEditMode(ObjectListEditWidget::UnassignObjects);
    m_nativeList->setTitle("Native file types");
    m_nativeList->setAssignList(Ramses::instance()->fileTypes());
    splitter->addWidget(m_nativeList);

    m_importList = new ObjectListEditWidget(true, RamUser::Admin, splitter);
    m_importList->setEditMode(ObjectListEditWidget::UnassignObjects);
    m_importList->setTitle("Imports");
    m_importList->setAssignList(Ramses::instance()->fileTypes());
    splitter->addWidget(m_importList);

    m_exportList = new ObjectListEditWidget(true, RamUser::Admin, splitter);
    m_exportList->setEditMode(ObjectListEditWidget::UnassignObjects);
    m_exportList->setTitle("Exports");
    m_exportList->setAssignList(Ramses::instance()->fileTypes());
    splitter->addWidget(m_exportList);

    mainLayout->addWidget(splitter);
}

