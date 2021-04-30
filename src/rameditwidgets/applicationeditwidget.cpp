#include "applicationeditwidget.h"

#include "objectlisteditwidget.h"

ApplicationEditWidget::ApplicationEditWidget(QWidget *parent) : ObjectEditWidget(parent)
{
    setupUi();
    populateMenus();
    connectEvents();

    setObject(nullptr);
}

ApplicationEditWidget::ApplicationEditWidget(RamApplication *app, QWidget *parent) : ObjectEditWidget(app, parent)
{
    setupUi();
    populateMenus();
    connectEvents();

    setObject(app);
}

RamApplication *ApplicationEditWidget::application() const
{
    return _application;
}

void ApplicationEditWidget::setObject(RamObject *obj)
{
    RamApplication *application = (RamApplication*)obj;
    this->setEnabled(false);

    ObjectEditWidget::setObject(application);
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
    for( int i = 0; i < application->nativeFileTypes()->count(); i++) nativeFileTypeAssigned( application->nativeFileTypes()->at(i) );
    for( int i = 0; i < application->importFileTypes()->count(); i++)  importFileTypeAssigned( application->importFileTypes()->at(i) );
    for( int i = 0; i < application->exportFileTypes()->count(); i++)  exportFileTypeAssigned( application->exportFileTypes()->at(i) );

    nativeList->clear(application->nativeFileTypes());
    importList->clear(application->importFileTypes());
    exportList->clear(application->exportFileTypes());

    _objectConnections << connect(application->nativeFileTypes(), &RamObjectList::objectAdded, this, &ApplicationEditWidget::nativeFileTypeAssigned);
    _objectConnections << connect(application->nativeFileTypes(), &RamObjectList::objectRemoved, this, &ApplicationEditWidget::nativeFileTypeUnassigned);
    _objectConnections << connect(application->importFileTypes(), &RamObjectList::objectAdded, this, &ApplicationEditWidget::nativeFileTypeAssigned);
    _objectConnections << connect(application->importFileTypes(), &RamObjectList::objectRemoved, this, &ApplicationEditWidget::nativeFileTypeUnassigned);
    _objectConnections << connect(application->exportFileTypes(), &RamObjectList::objectAdded, this, &ApplicationEditWidget::nativeFileTypeAssigned);
    _objectConnections << connect(application->exportFileTypes(), &RamObjectList::objectRemoved, this, &ApplicationEditWidget::nativeFileTypeUnassigned);
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
    setObject(_application);
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

    QList<QMetaObject::Connection> c;
    c << connect(ft, &RamObject::changed, this, &ApplicationEditWidget::fileTypeChanged);
    c << connect(ft, &RamObject::removed, this, &ApplicationEditWidget::fileTypeRemoved);
    m_fileTypeConnections[ft->uuid()] = c;
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
    if (m_fileTypeConnections.contains(o->uuid()))
    {
        QList<QMetaObject::Connection> c = m_fileTypeConnections.value( o->uuid() );
        while (!c.isEmpty()) disconnect( c.takeLast() );
    }

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
    _application->nativeFileTypes()->append(ft);
}

void ApplicationEditWidget::assignImportFileType()
{
    if (!_application) return;
    QAction *fileTypeAction = (QAction*)sender();
    RamFileType *ft = Ramses::instance()->fileType( fileTypeAction->data().toString() );
    if (!ft) return;
    _application->importFileTypes()->append(ft);
}

void ApplicationEditWidget::assignExportFileType()
{
    if (!_application) return;
    QAction *fileTypeAction = (QAction*)sender();
    RamFileType *ft = Ramses::instance()->fileType( fileTypeAction->data().toString() );
    if (!ft) return;
    _application->exportFileTypes()->append(ft);
}

void ApplicationEditWidget::nativeFileTypeAssigned(RamObject * const ft)
{
    if (!ft) return;
    if (ft->uuid() == "") return;

    //hide from assign list
    foreach (QAction *a, nativeMenu->actions()) if (a->data().toString() == ft->uuid()) a->setVisible(false);
}

void ApplicationEditWidget::importFileTypeAssigned(RamObject * const ft)
{
    if (!ft) return;
    if (ft->uuid() == "") return;

    //hide from assign list
    foreach (QAction *a, importMenu->actions()) if (a->data().toString() == ft->uuid()) a->setVisible(false);
}

void ApplicationEditWidget::exportFileTypeAssigned(RamObject * const ft)
{
    if (!ft) return;
    if (ft->uuid() == "") return;

    //hide from assign list
    foreach (QAction *a, exportMenu->actions()) if (a->data().toString() == ft->uuid()) a->setVisible(false);
}

void ApplicationEditWidget::nativeFileTypeUnassigned(RamObject * const ft)
{
    //show in assign list
    foreach(QAction *a, nativeMenu->actions())
        if (a->data().toString() == ft->uuid()) a->setVisible(true);
}

void ApplicationEditWidget::importFileTypeUnassigned(RamObject * const ft)
{
    //show in assign list
    foreach(QAction *a, importMenu->actions())
        if (a->data().toString() == ft->uuid()) a->setVisible(true);
}

void ApplicationEditWidget::exportFileTypeUnassigned(RamObject * const ft)
{
    //show in assign list
    foreach(QAction *a, exportMenu->actions())
        if (a->data().toString() == ft->uuid()) a->setVisible(true);
}

void ApplicationEditWidget::setupUi()
{
    QLabel *fileLabel = new QLabel("Executable file", this);
    mainFormLayout->addWidget(fileLabel, 2, 0);

    folderSelector = new DuQFFolderSelectorWidget(DuQFFolderSelectorWidget::File, this);
    mainFormLayout->addWidget(folderSelector, 2, 1);

    QSplitter *splitter = new QSplitter(this);
    splitter->setOrientation(Qt::Vertical);

    nativeList = new ObjectListEditWidget(true, splitter);
    nativeList->setTitle("Native file types");
    splitter->addWidget(nativeList);

    importList = new ObjectListEditWidget(true, splitter);
    importList->setTitle("Imports");
    splitter->addWidget(importList);

    exportList = new ObjectListEditWidget(true, splitter);
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
}

