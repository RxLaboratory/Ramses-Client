#include "applicationeditwidget.h"

ApplicationEditWidget::ApplicationEditWidget(QWidget *parent) : ObjectEditWidget(parent)
{
    setupUi();
    populateMenus();
    connectEvents();

    setApplication(nullptr);
}

ApplicationEditWidget::ApplicationEditWidget(RamApplication *app, QWidget *parent) : ObjectEditWidget(app, parent)
{
    setupUi();
    populateMenus();
    connectEvents();

    setApplication(app);
}

RamApplication *ApplicationEditWidget::application() const
{
    return _application;
}

void ApplicationEditWidget::setApplication(RamApplication *application)
{
    this->setEnabled(false);

    setObject(application);
    _application = application;

    QSignalBlocker b(folderSelector);
    QSignalBlocker b1(nativeList);
    QSignalBlocker b2(importList);
    QSignalBlocker b3(exportList);

    folderSelector->setPath("");
    nativeList->clear();
    importList->clear();
    exportList->clear();

    if (!application) return;

    folderSelector->setPath(application->executableFilePath());

    // Load file types
    QList<QAction *> actions = nativeMenu->actions();
    actions << exportMenu->actions();
    actions << importMenu->actions();
    foreach (QAction *a, actions) a->setVisible(true);
    foreach( RamFileType *ft, application->nativeFileTypes()) nativeFileTypeAssigned(ft);
    foreach( RamFileType *ft, application->importFileTypes()) importFileTypeAssigned(ft);
    foreach( RamFileType *ft, application->exportFileTypes()) exportFileTypeAssigned(ft);

    _objectConnections << connect(application, &RamApplication::nativeFileTypeAssigned, this, &ApplicationEditWidget::nativeFileTypeAssigned);
    _objectConnections << connect(application, &RamApplication::nativeFileTypeUnassigned, this, &ApplicationEditWidget::nativeFileTypeUnassigned);
    _objectConnections << connect(application, &RamApplication::importFileTypeAssigned, this, &ApplicationEditWidget::importFileTypeAssigned);
    _objectConnections << connect(application, &RamApplication::importFileTypeUnassigned, this, &ApplicationEditWidget::importFileTypeUnassigned);
    _objectConnections << connect(application, &RamApplication::exportFileTypeAssigned, this, &ApplicationEditWidget::exportFileTypeAssigned);
    _objectConnections << connect(application, &RamApplication::exportFileTypeUnassigned, this, &ApplicationEditWidget::exportFileTypeUnassigned);
    _objectConnections << connect(application, &RamApplication::changed, this, &ApplicationEditWidget::applicationChanged);

    this->setEnabled(Ramses::instance()->isProjectAdmin());
}

void ApplicationEditWidget::update()
{
    if (!_application) return;

    updating = true;

    _application->setExecutableFilePath(folderSelector->path());
    ObjectEditWidget::update();

    updating = false;
}

void ApplicationEditWidget::applicationChanged(RamObject *o)
{
    if (updating) return;
    Q_UNUSED(o);
    setApplication(_application);
}

void ApplicationEditWidget::newFileType(RamObject * const ft)
{
    if (!ft) return;
    if (ft->uuid() == "") return;
    QAction *assignNative = new QAction(ft->name());
    QAction *assignImport = new QAction(ft->name());
    QAction *assignExport = new QAction(ft->name());
    assignNative->setData(ft->uuid());
    assignImport->setData(ft->uuid());
    assignExport->setData(ft->uuid());
    nativeMenu->addAction(assignNative);
    importMenu->addAction(assignImport);
    exportMenu->addAction(assignExport);

    connect(ft, &RamUser::changed, this, &ApplicationEditWidget::fileTypeChanged);
    connect(ft, &RamUser::removed, this, &ApplicationEditWidget::fileTypeRemoved);
    connect(assignNative, &QAction::triggered, this, &ApplicationEditWidget::assignNativeFileType);
    connect(assignImport, &QAction::triggered, this, &ApplicationEditWidget::assignImportFileType);
    connect(assignExport, &QAction::triggered, this, &ApplicationEditWidget::assignExportFileType);
}

void ApplicationEditWidget::fileTypeChanged(RamObject *o)
{
    QList<QAction *> actions = nativeMenu->actions();
    actions << importMenu->actions();
    actions << exportMenu->actions();

    for(int i = actions.count() -1; i >= 0; i--)
    {
        if(actions.at(i)->data().toString() == o->uuid()) actions.at(i)->setText(o->name());
    }
}

void ApplicationEditWidget::fileTypeRemoved(RamObject *o)
{
    QList<QAction *> actions = nativeMenu->actions();
    actions << importMenu->actions();
    actions << exportMenu->actions();

    for(int i = actions.count() -1; i >= 0; i--)
    {
        if(actions.at(i)->data().toString() == o->uuid()) actions.at(i)->deleteLater();
    }
}

void ApplicationEditWidget::assignNativeFileType()
{
    if (!_application) return;
    QAction *fileTypeAction = (QAction*)sender();
    RamFileType *ft = Ramses::instance()->fileType( fileTypeAction->data().toString() );
    if (!ft) return;
    _application->assignNativeFileType(ft);
}

void ApplicationEditWidget::assignImportFileType()
{
    if (!_application) return;
    QAction *fileTypeAction = (QAction*)sender();
    RamFileType *ft = Ramses::instance()->fileType( fileTypeAction->data().toString() );
    if (!ft) return;
    _application->assignImportFileType(ft);
}

void ApplicationEditWidget::assignExportFileType()
{
    if (!_application) return;
    QAction *fileTypeAction = (QAction*)sender();
    RamFileType *ft = Ramses::instance()->fileType( fileTypeAction->data().toString() );
    if (!ft) return;
    _application->assignExportFileType(ft);
}

void ApplicationEditWidget::unassignNativeFileType(RamObject *o)
{
    if (!_application) return;
    _application->unassignNativeFileType( o );
}

void ApplicationEditWidget::unassignImportFileType(RamObject *o)
{
    if (!_application) return;
    _application->unassignImportFileType( o );
}

void ApplicationEditWidget::unassignExportFileType(RamObject *o)
{
    if (!_application) return;
    _application->unassignExportFileType( o );
}

void ApplicationEditWidget::nativeFileTypeAssigned(RamFileType * const ft)
{
    if (!ft) return;
    if (ft->uuid() == "") return;

    nativeList->addObject(ft);

    //hide from assign list
    foreach (QAction *a, nativeMenu->actions()) if (a->data().toString() == ft->uuid()) a->setVisible(false);
}

void ApplicationEditWidget::importFileTypeAssigned(RamFileType * const ft)
{
    if (!ft) return;
    if (ft->uuid() == "") return;

    importList->addObject(ft);

    //hide from assign list
    foreach (QAction *a, importMenu->actions()) if (a->data().toString() == ft->uuid()) a->setVisible(false);
}

void ApplicationEditWidget::exportFileTypeAssigned(RamFileType * const ft)
{
    if (!ft) return;
    if (ft->uuid() == "") return;

    exportList->addObject(ft);

    //hide from assign list
    foreach (QAction *a, exportMenu->actions()) if (a->data().toString() == ft->uuid()) a->setVisible(false);
}

void ApplicationEditWidget::nativeFileTypeUnassigned(QString uuid)
{
    nativeList->removeObject(uuid);

    //show in assign list
    foreach(QAction *a, nativeMenu->actions())
        if (a->data().toString() == uuid) a->setVisible(true);
}

void ApplicationEditWidget::importFileTypeUnassigned(QString uuid)
{
    importList->removeObject(uuid);

    //show in assign list
    foreach(QAction *a, importMenu->actions())
        if (a->data().toString() == uuid) a->setVisible(true);
}

void ApplicationEditWidget::exportFileTypeUnassigned(QString uuid)
{
    exportList->removeObject(uuid);

    //show in assign list
    foreach(QAction *a, exportMenu->actions())
        if (a->data().toString() == uuid) a->setVisible(true);
}

void ApplicationEditWidget::setupUi()
{
    QLabel *fileLabel = new QLabel("Executable file", this);
    mainFormLayout->addWidget(fileLabel, 2, 0);

    folderSelector = new DuQFFolderSelectorWidget(DuQFFolderSelectorWidget::File, this);
    mainFormLayout->addWidget(folderSelector, 2, 1);

    QSplitter *splitter = new QSplitter(this);
    splitter->setOrientation(Qt::Vertical);

    nativeList = new SimpleObjectList(true, splitter);
    nativeList->setTitle("Native file types");
    splitter->addWidget(nativeList);

    importList = new SimpleObjectList(true, splitter);
    importList->setTitle("Imports");
    splitter->addWidget(importList);

    exportList = new SimpleObjectList(true, splitter);
    exportList->setTitle("Exports");
    splitter->addWidget(exportList);

    mainLayout->addWidget(splitter);

    // Add menu
    nativeMenu = new QMenu(this);
    QToolButton *addNativeButton = nativeList->addButton();
    addNativeButton->setPopupMode(QToolButton::InstantPopup);
    addNativeButton->setMenu(nativeMenu);

    importMenu = new QMenu(this);
    QToolButton *addImportButton = importList->addButton();
    addImportButton->setPopupMode(QToolButton::InstantPopup);
    addImportButton->setMenu(importMenu);

    exportMenu = new QMenu(this);
    QToolButton *addExportButton = exportList->addButton();
    addExportButton->setPopupMode(QToolButton::InstantPopup);
    addExportButton->setMenu(exportMenu);
}

void ApplicationEditWidget::populateMenus()
{
    for (int i = 0; i < Ramses::instance()->fileTypes()->count(); i++) newFileType( Ramses::instance()->fileTypes()->at(i) );
}

void ApplicationEditWidget::connectEvents()
{
    connect(Ramses::instance()->fileTypes(), &RamObjectList::objectAdded, this, &ApplicationEditWidget::newFileType);
    connect(Ramses::instance()->fileTypes(), &RamObjectList::objectRemoved, this, &ApplicationEditWidget::fileTypeRemoved);
    connect(nativeList, &SimpleObjectList::objectRemoved, this, &ApplicationEditWidget::unassignNativeFileType);
    connect(importList, &SimpleObjectList::objectRemoved, this, &ApplicationEditWidget::unassignImportFileType);
    connect(exportList, &SimpleObjectList::objectRemoved, this, &ApplicationEditWidget::unassignExportFileType);
}

