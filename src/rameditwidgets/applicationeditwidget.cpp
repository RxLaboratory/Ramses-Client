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
    return m_application;
}

void ApplicationEditWidget::setObject(RamObject *obj)
{
    RamApplication *application = qobject_cast<RamApplication*>( obj );
    this->setEnabled(false);

    ObjectEditWidget::setObject(application);
    m_application = application;

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

    nativeList->setList(application->nativeFileTypes());
    importList->setList(application->importFileTypes());
    exportList->setList(application->exportFileTypes());

    _objectConnections << connect(application->nativeFileTypes(), SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(nativeFileTypeAssigned(QModelIndex,int,int)));
    _objectConnections << connect(application->nativeFileTypes(), SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)), this, SLOT(nativeFileTypeUnassigned(QModelIndex,int,int)));

    _objectConnections << connect(application->importFileTypes(), SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(importFileTypeAssigned(QModelIndex,int,int)));
    _objectConnections << connect(application->importFileTypes(), SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)), this, SLOT(importFileTypeUnassigned(QModelIndex,int,int)));

    _objectConnections << connect(application->exportFileTypes(), SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(exportFileTypeAssigned(QModelIndex,int,int)));
    _objectConnections << connect(application->exportFileTypes(), SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)), this, SLOT(exportFileTypeUnassigned(QModelIndex,int,int)));

    this->setEnabled(Ramses::instance()->isProjectAdmin());
}

void ApplicationEditWidget::update()
{
    if (!m_application) return;

    updating = true;

    m_application->setExecutableFilePath(folderSelector->path());
    ObjectEditWidget::update();

    updating = false;
}

void ApplicationEditWidget::newFileType(const QModelIndex &parent,int first,int last)
{
    Q_UNUSED(parent)

    for (int i = first; i <= last; i++)
    {
        RamObject *ft = Ramses::instance()->fileTypes()->at(i);
        newFileType(ft);
    }
}

void ApplicationEditWidget::newFileType(RamObject *fileTypeObj)
{
    QAction *assignNative = new QAction(fileTypeObj->name());
    QAction *assignImport = new QAction(fileTypeObj->name());
    QAction *assignExport = new QAction(fileTypeObj->name());
    assignNative->setData(fileTypeObj->uuid());
    assignImport->setData(fileTypeObj->uuid());
    assignExport->setData(fileTypeObj->uuid());
    nativeMenu->addAction(assignNative);
    importMenu->addAction(assignImport);
    exportMenu->addAction(assignExport);

    connect(assignNative, &QAction::triggered, this, &ApplicationEditWidget::assignNativeFileType);
    connect(assignImport, &QAction::triggered, this, &ApplicationEditWidget::assignImportFileType);
    connect(assignExport, &QAction::triggered, this, &ApplicationEditWidget::assignExportFileType);
}

void ApplicationEditWidget::fileTypeChanged(const QModelIndex &first, const QModelIndex &last)
{
    QList<QAction *> actions = nativeMenu->actions();
    actions << importMenu->actions();
    actions << exportMenu->actions();


    for( int i = first.row(); i <= last.row(); i++)
    {
        RamObject *o = Ramses::instance()->fileTypes()->at(i);

        for(int i = actions.count() -1; i >= 0; i--)
        {
            if(actions.at(i)->data().toString() == o->uuid()) actions.at(i)->setText(o->name());
        }
    }


}

void ApplicationEditWidget::fileTypeRemoved(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent)

    QList<QAction *> actions = nativeMenu->actions();
    actions << importMenu->actions();
    actions << exportMenu->actions();

    for (int i = first; i <= last; i++)
    {
        RamObject *ft = Ramses::instance()->fileTypes()->at(i);
        if(actions.at(i)->data().toString() == ft->uuid()) actions.at(i)->deleteLater();

    }
}

void ApplicationEditWidget::assignNativeFileType()
{
    if (!m_application) return;
    QAction *fileTypeAction = (QAction*)sender();
    RamObject *ft = Ramses::instance()->fileTypes()->fromUuid( fileTypeAction->data().toString() );
    if (!ft) return;
    m_application->nativeFileTypes()->append(ft);
}

void ApplicationEditWidget::assignImportFileType()
{
    if (!m_application) return;
    QAction *fileTypeAction = (QAction*)sender();
    RamObject *ft = Ramses::instance()->fileTypes()->fromUuid( fileTypeAction->data().toString() );
    if (!ft) return;
    m_application->importFileTypes()->append(ft);
}

void ApplicationEditWidget::assignExportFileType()
{
    if (!m_application) return;
    QAction *fileTypeAction = (QAction*)sender();
    RamObject *ft = Ramses::instance()->fileTypes()->fromUuid( fileTypeAction->data().toString() );
    if (!ft) return;
    m_application->exportFileTypes()->append(ft);
}

void ApplicationEditWidget::nativeFileTypeAssigned(const QModelIndex &parent,int first,int last)
{
    Q_UNUSED(parent)

    for (int i = first ; i <= last; i++)
    {
        RamObject *ftObj = m_application->nativeFileTypes()->at(i);
        foreach (QAction *a, nativeMenu->actions())
            if (a->data().toString() == ftObj->uuid())
                a->setVisible(false);
    }
}

void ApplicationEditWidget::importFileTypeAssigned(const QModelIndex &parent,int first,int last)
{
    Q_UNUSED(parent)

    for (int i = first ; i <= last; i++)
    {
        RamObject *ftObj = m_application->importFileTypes()->at(i);
        foreach (QAction *a, importMenu->actions())
            if (a->data().toString() == ftObj->uuid())
                a->setVisible(false);
    }
}

void ApplicationEditWidget::exportFileTypeAssigned(const QModelIndex &parent,int first,int last)
{
    Q_UNUSED(parent)

    for (int i = first ; i <= last; i++)
    {
        RamObject *ftObj = m_application->exportFileTypes()->at(i);
        foreach (QAction *a, exportMenu->actions())
            if (a->data().toString() == ftObj->uuid())
                a->setVisible(false);
    }
}

void ApplicationEditWidget::nativeFileTypeUnassigned(const QModelIndex &parent,int first,int last)
{
    Q_UNUSED(parent)

    for (int i = first ; i <= last; i++)
    {
        RamObject *ftObj = m_application->nativeFileTypes()->at(i);
        foreach (QAction *a, nativeMenu->actions())
            if (a->data().toString() == ftObj->uuid())
                a->setVisible(true);
    }
}

void ApplicationEditWidget::importFileTypeUnassigned(const QModelIndex &parent,int first,int last)
{
    Q_UNUSED(parent)

    for (int i = first ; i <= last; i++)
    {
        RamObject *ftObj = m_application->importFileTypes()->at(i);
        foreach (QAction *a, importMenu->actions())
            if (a->data().toString() == ftObj->uuid())
                a->setVisible(true);
    }
}

void ApplicationEditWidget::exportFileTypeUnassigned(const QModelIndex &parent,int first,int last)
{
    Q_UNUSED(parent)

    for (int i = first ; i <= last; i++)
    {
        RamObject *ftObj = m_application->exportFileTypes()->at(i);
        foreach (QAction *a, exportMenu->actions())
            if (a->data().toString() == ftObj->uuid())
                a->setVisible(true);
    }
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
    nativeList->setEditMode(RamObjectListWidget::UnassignObjects);
    nativeList->setTitle("Native file types");
    splitter->addWidget(nativeList);

    importList = new ObjectListEditWidget(true, splitter);
    importList->setEditMode(RamObjectListWidget::UnassignObjects);
    importList->setTitle("Imports");
    splitter->addWidget(importList);

    exportList = new ObjectListEditWidget(true, splitter);
    exportList->setEditMode(RamObjectListWidget::UnassignObjects);
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
    connect(Ramses::instance()->fileTypes(), SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(newFileType(QModelIndex,int,int)));
    connect(Ramses::instance()->fileTypes(), SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)), this, SLOT(fileTypeRemoved(QModelIndex,int,int)));
    connect(Ramses::instance()->fileTypes(), SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)), this, SLOT(fileTypeChanged(QModelIndex,QModelIndex)));
}

