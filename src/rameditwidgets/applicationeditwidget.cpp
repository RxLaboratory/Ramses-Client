#include "applicationeditwidget.h"

#include "objectlisteditwidget.h"

ApplicationEditWidget::ApplicationEditWidget(QWidget *parent) : ObjectEditWidget(parent)
{
    setupUi();
    setObject(nullptr);
    connectEvents();
}

ApplicationEditWidget::ApplicationEditWidget(RamApplication *app, QWidget *parent) : ObjectEditWidget(app, parent)
{
    setupUi();
    setObject(app);
    connectEvents();
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

void ApplicationEditWidget::createForNative()
{
    if (!m_application) return;
    RamFileType *ft = new RamFileType(
                "NEW",
                "New file type");
    Ramses::instance()->fileTypes()->append(ft);
    m_application->nativeFileTypes()->append(ft);
    ft->edit();
}

void ApplicationEditWidget::createForImport()
{
    qDebug() << "CREATE";
    if (!m_application) return;
    RamFileType *ft = new RamFileType(
                "NEW",
                "New file type");
    Ramses::instance()->fileTypes()->append(ft);
    m_application->importFileTypes()->append(ft);
    ft->edit();
}

void ApplicationEditWidget::createForExport()
{
    if (!m_application) return;
    RamFileType *ft = new RamFileType(
                "NEW",
                "New file type");
    Ramses::instance()->fileTypes()->append(ft);
    m_application->exportFileTypes()->append(ft);
    ft->edit();
}

void ApplicationEditWidget::setupUi()
{
    QLabel *fileLabel = new QLabel("Executable file", this);
    ui_mainFormLayout->addWidget(fileLabel, 3, 0);

    m_folderSelector = new DuQFFolderSelectorWidget(DuQFFolderSelectorWidget::File, this);
    ui_mainFormLayout->addWidget(m_folderSelector, 3, 1);

    QTabWidget *tabWidget = new QTabWidget(this);

    m_nativeList = new ObjectListEditWidget(true, RamUser::AdminFolder);
    m_nativeList->setEditMode(ObjectListEditWidget::UnassignObjects);
    m_nativeList->setTitle("Native file types");
    m_nativeList->setAssignList(Ramses::instance()->fileTypes());
    tabWidget->addTab(m_nativeList, QIcon(":/icons/files"), "Native");

    m_importList = new ObjectListEditWidget(true, RamUser::AdminFolder);
    m_importList->setEditMode(ObjectListEditWidget::UnassignObjects);
    m_importList->setTitle("Imports");
    m_importList->setAssignList(Ramses::instance()->fileTypes());
    tabWidget->addTab(m_importList, QIcon(":/icons/files"), "Import");

    m_exportList = new ObjectListEditWidget(true, RamUser::AdminFolder);
    m_exportList->setEditMode(ObjectListEditWidget::UnassignObjects);
    m_exportList->setTitle("Exports");
    m_exportList->setAssignList(Ramses::instance()->fileTypes());
    tabWidget->addTab(m_exportList, QIcon(":/icons/files"), "Export");

    ui_mainLayout->addWidget(tabWidget);

    ui_mainLayout->setStretch(2, 100);
}

void ApplicationEditWidget::connectEvents()
{
    connect(m_nativeList, SIGNAL(add()), this, SLOT(createForNative()));
    connect(m_importList, SIGNAL(add()), this, SLOT(createForImport()));
    connect(m_exportList, SIGNAL(add()), this, SLOT(createForExport()));
}

