#include "applicationeditwidget.h"

ApplicationEditWidget::ApplicationEditWidget(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);

    folderSelector = new DuQFFolderSelectorWidget(DuQFFolderSelectorWidget::File, this);
    executableLayout->addWidget(folderSelector);

    // Add menus
    nativeMenu = new QMenu(this);
    assignNativeButton->setMenu(nativeMenu);
    importMenu = new QMenu(this);
    assignImportButton->setMenu(importMenu);
    exportMenu = new QMenu(this);
    assignExportButton->setMenu(exportMenu);


    connect(updateButton, SIGNAL(clicked()), this, SLOT(update()));
    connect(revertButton, SIGNAL(clicked()), this, SLOT(revert()));
    connect(shortNameEdit, &QLineEdit::textChanged, this, &ApplicationEditWidget::checkInput);
    connect(DBInterface::instance(),&DBInterface::newLog, this, &ApplicationEditWidget::dbiLog);
    connect(Ramses::instance(), &Ramses::newFileType, this, &ApplicationEditWidget::newFileType);
    connect(removeNativeButton, &QToolButton::clicked, this, &ApplicationEditWidget::unassignNativeFileType);
    connect(removeImportButton, &QToolButton::clicked, this, &ApplicationEditWidget::unassignImportFileType);
    connect(removeExportButton, &QToolButton::clicked, this, &ApplicationEditWidget::unassignExportFileType);

    this->setEnabled(false);
}

RamApplication *ApplicationEditWidget::application() const
{
    return _application;
}

void ApplicationEditWidget::setApplication(RamApplication *application)
{
    while (_applicationConnections.count() > 0) disconnect(_applicationConnections.takeLast());

    _application = application;

    nameEdit->setText("");
    shortNameEdit->setText("");
    folderSelector->setPath("");

    if (!application) return;

    nameEdit->setText(application->name());
    shortNameEdit->setText(application->shortName());
    folderSelector->setPath(application->executableFilePath());

    // Load file types
    nativeList->clear();
    importList->clear();
    exportList->clear();
    QList<QAction *> actions = nativeMenu->actions();
    actions << exportMenu->actions();
    actions << importMenu->actions();
    foreach (QAction *a, actions) a->setVisible(true);
    foreach( RamFileType *ft, application->nativeFileTypes()) nativeFileTypeAssigned(ft);
    foreach( RamFileType *ft, application->importFileTypes()) importFileTypeAssigned(ft);
    foreach( RamFileType *ft, application->exportFileTypes()) exportFileTypeAssigned(ft);

    _applicationConnections << connect(application, &RamObject::removed, this, &ApplicationEditWidget::applicationRemoved);
    _applicationConnections << connect(application, &RamApplication::nativeFileTypeAssigned, this, &ApplicationEditWidget::nativeFileTypeAssigned);
    _applicationConnections << connect(application, &RamApplication::nativeFileTypeUnassigned, this, &ApplicationEditWidget::nativeFileTypeUnassigned);
    _applicationConnections << connect(application, &RamApplication::importFileTypeAssigned, this, &ApplicationEditWidget::importFileTypeAssigned);
    _applicationConnections << connect(application, &RamApplication::importFileTypeUnassigned, this, &ApplicationEditWidget::importFileTypeUnassigned);
    _applicationConnections << connect(application, &RamApplication::exportFileTypeAssigned, this, &ApplicationEditWidget::exportFileTypeAssigned);
    _applicationConnections << connect(application, &RamApplication::exportFileTypeUnassigned, this, &ApplicationEditWidget::exportFileTypeUnassigned);

    this->setEnabled(Ramses::instance()->isProjectAdmin());
}

void ApplicationEditWidget::update()
{
    if (!_application) return;

    this->setEnabled(false);

    if (!checkInput())
    {
        this->setEnabled(true);
        return;
    }

    _application->setName(nameEdit->text());
    _application->setShortName(shortNameEdit->text());
    _application->setExecutableFilePath(folderSelector->path());

    _application->update();

    this->setEnabled(true);
}

void ApplicationEditWidget::revert()
{
    setApplication(_application);
}

bool ApplicationEditWidget::checkInput()
{
    if (!_application) return false;

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

void ApplicationEditWidget::applicationRemoved(RamObject *o)
{
    Q_UNUSED(o);
    setApplication(nullptr);
}

void ApplicationEditWidget::newFileType(RamFileType * const ft)
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

void ApplicationEditWidget::fileTypeChanged()
{
    RamFileType *ft = (RamFileType*)sender();

    QList<QAction *> actions = nativeMenu->actions();
    actions << importMenu->actions();
    actions << exportMenu->actions();

    for(int i = actions.count() -1; i >= 0; i--)
    {
        if(actions.at(i)->data().toString() == ft->uuid()) actions.at(i)->setText(ft->name());
    }

    for(int i = nativeList->count() -1; i >= 0; i--)
    {
        if (nativeList->item(i)->data(Qt::UserRole).toString() == ft->uuid()) nativeList->item(i)->setText(ft->name());
    }
    for(int i = importList->count() -1; i >= 0; i--)
    {
        if (importList->item(i)->data(Qt::UserRole).toString() == ft->uuid()) importList->item(i)->setText(ft->name());
    }
    for(int i = exportList->count() -1; i >= 0; i--)
    {
        if (exportList->item(i)->data(Qt::UserRole).toString() == ft->uuid()) exportList->item(i)->setText(ft->name());
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

    nativeFileTypeUnassigned(o->uuid());
    importFileTypeUnassigned(o->uuid());
    exportFileTypeUnassigned(o->uuid());
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

void ApplicationEditWidget::unassignNativeFileType()
{
    if (!_application) return;
    _application->unassignNativeFileType( nativeList->currentItem()->data(Qt::UserRole).toString() );
}

void ApplicationEditWidget::unassignImportFileType()
{
    if (!_application) return;
    _application->unassignImportFileType( importList->currentItem()->data(Qt::UserRole).toString() );
}

void ApplicationEditWidget::unassignExportFileType()
{
    if (!_application) return;
    _application->unassignExportFileType( exportList->currentItem()->data(Qt::UserRole).toString() );
}

void ApplicationEditWidget::nativeFileTypeAssigned(RamFileType * const ft)
{
    if (!ft) return;
    if (ft->uuid() == "") return;

    QListWidgetItem *ftItem = new QListWidgetItem(ft->name());
    ftItem->setData(Qt::UserRole, ft->uuid());
    nativeList->addItem(ftItem);
    connect(ft, &RamFileType::removed, this, &ApplicationEditWidget::fileTypeRemoved);
    connect(ft, &RamFileType::changed, this, &ApplicationEditWidget::fileTypeChanged);

    //hide from assign list
    foreach (QAction *a, nativeMenu->actions()) if (a->data().toString() == ft->uuid()) a->setVisible(false);
}

void ApplicationEditWidget::importFileTypeAssigned(RamFileType * const ft)
{
    if (!ft) return;
    if (ft->uuid() == "") return;

    QListWidgetItem *ftItem = new QListWidgetItem(ft->name());
    ftItem->setData(Qt::UserRole, ft->uuid());
    importList->addItem(ftItem);
    connect(ft, &RamFileType::removed, this, &ApplicationEditWidget::fileTypeRemoved);
    connect(ft, &RamFileType::changed, this, &ApplicationEditWidget::fileTypeChanged);

    //hide from assign list
    foreach (QAction *a, importMenu->actions()) if (a->data().toString() == ft->uuid()) a->setVisible(false);
}

void ApplicationEditWidget::exportFileTypeAssigned(RamFileType * const ft)
{
    if (!ft) return;
    if (ft->uuid() == "") return;

    QListWidgetItem *ftItem = new QListWidgetItem(ft->name());
    ftItem->setData(Qt::UserRole, ft->uuid());
    exportList->addItem(ftItem);
    connect(ft, &RamFileType::removed, this, &ApplicationEditWidget::fileTypeRemoved);
    connect(ft, &RamFileType::changed, this, &ApplicationEditWidget::fileTypeChanged);

    //hide from assign list
    foreach (QAction *a, exportMenu->actions()) if (a->data().toString() == ft->uuid()) a->setVisible(false);
}

void ApplicationEditWidget::nativeFileTypeUnassigned(QString uuid)
{
    //remove from list
    for(int i = nativeList->count() -1; i >= 0; i--)
        if (nativeList->item(i)->data(Qt::UserRole).toString() == uuid)
            delete nativeList->takeItem(i);

    //show in assign list
    foreach(QAction *a, nativeMenu->actions())
        if (a->data().toString() == uuid) a->setVisible(true);
}

void ApplicationEditWidget::importFileTypeUnassigned(QString uuid)
{
    //remove from list
    for(int i = importList->count() -1; i >= 0; i--)
        if (importList->item(i)->data(Qt::UserRole).toString() == uuid)
            delete importList->takeItem(i);

    //show in assign list
    foreach(QAction *a, importMenu->actions())
        if (a->data().toString() == uuid) a->setVisible(true);
}

void ApplicationEditWidget::exportFileTypeUnassigned(QString uuid)
{
    //remove from list
    for(int i = exportList->count() -1; i >= 0; i--)
        if (exportList->item(i)->data(Qt::UserRole).toString() == uuid)
            delete exportList->takeItem(i);

    //show in assign list
    foreach(QAction *a, exportMenu->actions())
        if (a->data().toString() == uuid) a->setVisible(true);
}

void ApplicationEditWidget::dbiLog(DuQFLog m)
{
    if (m.type() != DuQFLog::Debug) statusLabel->setText(m.message());
}
