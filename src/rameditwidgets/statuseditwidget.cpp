#include "statuseditwidget.h"

StatusEditWidget::StatusEditWidget(QWidget *parent) :
    ObjectEditWidget( parent)
{
    setupUi();
    connectEvents();
    setStatus(nullptr);
}

StatusEditWidget::StatusEditWidget(RamStatus *status, QWidget *parent) : ObjectEditWidget(status, parent)
{
    setupUi();
    connectEvents();
    setStatus(status);
}

void StatusEditWidget::setStatus(RamStatus *status)
{
    m_status = status;

    QSignalBlocker b1(ui_stateBox);
    QSignalBlocker b2(ui_completionBox);
    QSignalBlocker b3(ui_versionBox);
    QSignalBlocker b4(ui_statusCommentEdit);
    QSignalBlocker b5(ui_publishedBox);
    QSignalBlocker b6(ui_timeSpent);
    QSignalBlocker b7(ui_mainFileList);
    QSignalBlocker b8(ui_publishedFileList);
    QSignalBlocker b9(ui_previewFileList);
    QSignalBlocker b10(ui_folderWidget);
    QSignalBlocker b11(ui_difficultyBox);
    QSignalBlocker b12(ui_autoEstimationBox);
    QSignalBlocker b13(ui_userBox);
    QSignalBlocker b14(ui_estimationEdit);

    ui_stateBox->setCurrentText("STB");
    ui_completionBox->setValue(0);
    ui_versionBox->setValue(1);
    ui_statusCommentEdit->setPlainText("");
    ui_publishedBox->setChecked(false);
    ui_userBox->setObject("");
    ui_timeSpent->setValue(0);
    ui_mainFileList->clear();
    ui_publishedFileList->clear();
    ui_previewFileList->clear();
    ui_folderWidget->setPath("");
    ui_difficultyBox->setCurrentIndex(2);
    ui_autoEstimationBox->setChecked(true);
    ui_estimationEdit->setValue(0);

    // Remove template list
    QList<QAction*> templateActions = ui_createFromTemplateMenu->actions();
    for (int i = 2; i < templateActions.count(); i++)
    {
        templateActions.at(i)->deleteLater();
    }

    if (!status) return;

    ui_stateBox->setObject(status->state());
    ui_completionBox->setValue(status->completionRatio());
    ui_versionBox->setValue(status->version());
    ui_statusCommentEdit->setPlainText(status->comment());
    ui_folderWidget->setPath( status->path() );
    ui_publishedBox->setChecked( status->isPublished() );

    // Get users from project
    RamProject *project = nullptr;
    if (status->item()) project = status->item()->project();
    if (!project && status->step()) project = status->step()->project();
    if (project) ui_userBox->setList( project->users() );

    ui_userBox->setObject(status->assignedUser());

    // Try to auto compute time spent from previous status
    qint64 timeSpent = status->timeSpent();
    if (!status->isTimeSpentManual() || timeSpent == 0)
    {
        RamStepStatusHistory *history = status->item()->statusHistory( status->step() );
        if (history->count() > 1)
        {
            RamStatus *previous = qobject_cast<RamStatus*>( history->at( history->count() -2) );
            timeSpent = previous->timeSpent();
            RamFileMetaDataManager mdm( status->path(RamObject::VersionsFolder ));
            if (mdm.isValid())
            {
                timeSpent += mdm.getTimeRange( previous->date() );
            }
        }
    }

    // (try to) Auto-detect version
    int v = status->latestVersion("");
    if (v > status->version()) ui_versionBox->setValue(v);

    // List files
    foreach(QString file, status->mainFiles())
    {
        QListWidgetItem *item = new QListWidgetItem(file, ui_mainFileList);
        item->setIcon(QIcon(":/icons/file"));
    }
    foreach(QString file, status->publishedFiles())
    {
        QListWidgetItem *item = new QListWidgetItem(file, ui_publishedFileList);
        item->setIcon(QIcon(":/icons/file"));
    }
    foreach(QString file, status->previewFiles())
    {
        QListWidgetItem *item = new QListWidgetItem(file, ui_previewFileList);
        item->setIcon(QIcon(":/icons/file"));
    }

    // List templates
    QStringList templateFiles =status->step()->publishedTemplates();
    ui_createMainFileButton->setEnabled( templateFiles.count() > 0);
    foreach(QString file, templateFiles)
    {
        QAction *action = new QAction(file);
        ui_createFromTemplateMenu->addAction(action);
        connect(action,SIGNAL(triggered()), this, SLOT(createFromTemplate()));
    }

    // Estimation
    ui_difficultyBox->setCurrentIndex( status->difficulty() );
    ui_autoEstimationBox->setChecked( status->estimation() <= 0 );
    if (!ui_autoEstimationBox->isChecked())
    {
        ui_estimationEdit->setEnabled(true);
        ui_estimationEdit->setValue( status->estimation() );
    }
    else
    {
        ui_estimationEdit->setEnabled(false);
        autoEstimate(true);
    }

    ui_timeSpent->setValue( timeSpent / 3600 );
}

RamState *StatusEditWidget::state() const
{
    return qobject_cast<RamState*>( ui_stateBox->currentObject() );
}

int StatusEditWidget::completionRatio() const
{
    return ui_completionBox->value();
}

int StatusEditWidget::version() const
{
    return ui_versionBox->value();
}

QString StatusEditWidget::comment() const
{
    return ui_statusCommentEdit->toPlainText();
}

RamUser *StatusEditWidget::assignedUser() const
{
    return qobject_cast<RamUser*>( ui_userBox->currentObject() );
}

bool StatusEditWidget::isPublished() const
{
    return ui_publishedBox->isChecked();
}

qint64 StatusEditWidget::timeSpent() const
{
    return ui_timeSpent->value() * 3600;
}

float StatusEditWidget::estimation() const
{
    if (ui_autoEstimationBox->isChecked()) return -1;
    return ui_estimationEdit->value();
}

RamStatus::Difficulty StatusEditWidget::difficulty() const
{
    switch(ui_difficultyBox->currentIndex())
    {
        case 0: return RamStatus::VeryEasy;
        case 1: return RamStatus::Easy;
        case 2: return RamStatus::Medium;
        case 3: return RamStatus::Hard;
        case 4: return RamStatus::VeryHard;
    }
    return RamStatus::Medium;
}

void StatusEditWidget::update()
{
    if (!m_status) return;

    updating = true;

    m_status->setState( state() );
    m_status->setCompletionRatio( completionRatio() );
    m_status->setVersion( version() );
    m_status->setComment( comment() );
    m_status->assignUser( assignedUser() );
    m_status->setPublished( isPublished() );
    m_status->setTimeSpent( timeSpent() );
    m_status->setEstimation( estimation() );
    m_status->setDifficulty( difficulty() );

    m_status->update();

    updating = false;
}

void StatusEditWidget::currentStateChanged(RamObject *stateObj)
{
    RamState *state = qobject_cast<RamState*>(stateObj);
    if (state)
    {
        ui_completionBox->setValue(state->completionRatio());
    }
    else
    {
        ui_completionBox->setValue(50);
    }
}

void StatusEditWidget::revert()
{
    setStatus(m_status);
}

void StatusEditWidget::checkPublished( int v )
{
    bool p = m_status->checkPublished( v );
    ui_publishedBox->setChecked(p);
}

void StatusEditWidget::mainFileSelected(int row)
{
    if (row < 0)
    {
        ui_versionFileBox->setEnabled(false);
        ui_openMainFileButton->setEnabled(false);
        return;
    }

    ui_versionFileBox->setEnabled(true);
    ui_openMainFileButton->setEnabled(true);

    // List versions
    QString fileName = ui_mainFileList->item(row)->text();
    RamNameManager nm;
    nm.setFileName(fileName);
    QString resource = nm.resource();

    RamFileMetaDataManager mdm(m_status->path(RamObject::VersionsFolder));

    ui_versionFileBox->clear();
    ui_versionFileBox->addItem("Current version", "");

    foreach(QString file, m_status->versionFiles(resource))
    {
        nm.setFileName(file);
        QString title = "v" + QString::number(nm.version()) + " | " + nm.state();
        // Retrieve comment if any
        QString comment = mdm.getComment(file);
        if (comment != "") title += " | " + comment;
        ui_versionFileBox->addItem(title, file);
    }
}

void StatusEditWidget::openMainFile()
{
    int row = ui_mainFileList->currentRow();
    if (row < 0) return;

    QString filePathToOpen;

    // If current version, open the main file
    int versionIndex = ui_versionFileBox->currentIndex();
    if (versionIndex < 1)
        filePathToOpen = QDir(
                    m_status->path()
                    ).filePath( ui_mainFileList->item(row)->text() );
    // Else, copy/rename from the versions folder
    else
        filePathToOpen = m_status->restoreVersionFile(
                    ui_versionFileBox->currentData(Qt::UserRole).toString()
                    );

    m_status->step()->openFile( filePathToOpen );

    DuQFLogger::instance()->log("Opening " + filePathToOpen + "...");

    revert();
}

void StatusEditWidget::removeSelectedMainFile()
{
    int row = ui_mainFileList->currentRow();
    if (row < 0) return;

    QString fileName = ui_mainFileList->item(row)->text();

    QMessageBox::StandardButton confirm = QMessageBox::question( this,
        "Confirm deletion",
        "Are you sure you want to delete the selected file?\n► " + fileName );
    if ( confirm != QMessageBox::Yes ) return;

    m_status->deleteFile( fileName );

    DuQFLogger::instance()->log("Deleting " + fileName + "...");

    revert();
}

void StatusEditWidget::createFromTemplate()
{
    QAction *action = qobject_cast<QAction*>( sender() );
    QString fileName = action->text();

    DuQFLogger::instance()->log("Creating " + fileName + "...");

    QString templateFile = m_status->createFileFromTemplate( fileName );

    DuQFLogger::instance()->log("Opening " + templateFile + "...");

    if (templateFile != "") m_status->step()->openFile(templateFile);

    revert();
}

void StatusEditWidget::createFromDefaultTemplate()
{

    DuQFLogger::instance()->log("Creating from default template...");

    QString templateFile = m_status->createFileFromTemplate( "" );

    if (templateFile == "")
    {
        DuQFLogger::instance()->log("Default template file not found!");
        return;
    }

    DuQFLogger::instance()->log("Opening " + templateFile + "...");

    if (templateFile != "") m_status->step()->openFile(templateFile);

    revert();
}

void StatusEditWidget::publishedFileSelected(int row)
{
    ui_openPublishedFileButton->setEnabled(row >= 0);
}

void StatusEditWidget::openPublishedFile()
{
    int row = ui_publishedFileList->currentRow();
    if (row < 0) return;

    QString filePathToOpen = QDir(
                m_status->path( RamObject::PublishFolder )
                ).filePath( ui_publishedFileList->item(row)->text() );

    m_status->step()->openFile( filePathToOpen );

    DuQFLogger::instance()->log("Opening " + filePathToOpen + "...");

    revert();
}

void StatusEditWidget::removeSelectedPublishedFile()
{
    int row = ui_publishedFileList->currentRow();
    if (row < 0) return;

    QString fileName = ui_publishedFileList->item(row)->text();

    QMessageBox::StandardButton confirm = QMessageBox::question( this,
        "Confirm deletion",
        "Are you sure you want to delete the selected file?\n► " + fileName );
    if ( confirm != QMessageBox::Yes ) return;

    m_status->deleteFile( fileName, RamObject::PublishFolder );

    DuQFLogger::instance()->log("Deleting " + fileName + "...");

    revert();
}

void StatusEditWidget::previewFileSelected(int row)
{
    ui_openPreviewFileButton->setEnabled(row >= 0);
}

void StatusEditWidget::openPreviewFile()
{
    int row = ui_previewFileList->currentRow();
    if (row < 0) return;

    QString filePathToOpen = QDir(
                m_status->path( RamObject::PreviewFolder )
                ).filePath( ui_previewFileList->item(row)->text() );

    // Try with the default app on the system
    QDesktopServices::openUrl(QUrl("file:///" + filePathToOpen));

    DuQFLogger::instance()->log("Opening " + filePathToOpen + "...");

    revert();
}

void StatusEditWidget::removeSelectedPreviewFile()
{
    int row = ui_previewFileList->currentRow();
    if (row < 0) return;

    QString fileName = ui_previewFileList->item(row)->text();

    QMessageBox::StandardButton confirm = QMessageBox::question( this,
        "Confirm deletion",
        "Are you sure you want to delete the selected file?\n► " + fileName );
    if ( confirm != QMessageBox::Yes ) return;

    m_status->deleteFile( fileName, RamObject::PreviewFolder );

    DuQFLogger::instance()->log("Deleting " + fileName + "...");

    revert();
}

void StatusEditWidget::autoEstimate(bool estimate)
{
    if (estimate)
    {
        float daysEstimation = m_status->autoEstimation( ui_difficultyBox->currentIndex() );
        ui_estimationEdit->setValue( daysEstimation );
    }

    ui_estimationEdit->setEnabled(!estimate);
}

void StatusEditWidget::autoEstimate()
{
    autoEstimate( ui_autoEstimationBox->isChecked() );
}

void StatusEditWidget::estimateDays(int hours)
{
    float days = RamStatus::hoursToDays(hours);
    ui_timeSpent->setSuffix(" hours (" + QString::number(days, 'f', 2) + " days)");
}

void StatusEditWidget::setupUi()
{
    this->hideName();
    ui_commentEdit->hide();
    ui_commentLabel->hide();

    QHBoxLayout *cLayout = new QHBoxLayout();
    cLayout->setContentsMargins(0,0,0,0);
    cLayout->setSpacing(3);

    ui_stateBox = new StateBox(this);
    cLayout->addWidget(ui_stateBox);

    ui_completionBox = new DuQFSpinBox(this);
    ui_completionBox->setMaximum(100);
    ui_completionBox->setMinimum(0);
    ui_completionBox->setSuffix("%");
    ui_completionBox->setValue(50);
    ui_completionBox->setFixedHeight(ui_stateBox->height());
    cLayout->addWidget(ui_completionBox);

    cLayout->setStretch(0,0);
    cLayout->setStretch(1,100);

    ui_mainLayout->insertLayout(0, cLayout);

    QSplitter *commentSplitter = new QSplitter(this);
    commentSplitter->setOrientation(Qt::Vertical);
    ui_mainLayout->addWidget( commentSplitter );

    ui_statusCommentEdit = new QPlainTextEdit(this);
    ui_statusCommentEdit->setPlaceholderText("Comment...");
    ui_statusCommentEdit->setMinimumHeight(30);
    ui_statusCommentEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui_commentEdit->setObjectName("commentEdit");

    commentSplitter->addWidget( ui_statusCommentEdit );

    QWidget *bottomWidget = new QWidget(this);
    QVBoxLayout *bottomLayout = new QVBoxLayout(bottomWidget);
    bottomLayout->setSpacing(0);
    bottomLayout->setContentsMargins(0,0,0,0);

    QFormLayout *detailsLayout = new QFormLayout();
    bottomLayout->addLayout(detailsLayout);

    ui_difficultyBox = new QComboBox(this);
    ui_difficultyBox->addItem("Very easy");
    ui_difficultyBox->addItem("Easy");
    ui_difficultyBox->addItem("Medium");
    ui_difficultyBox->addItem("Hard");
    ui_difficultyBox->addItem("Very hard");
    detailsLayout->addRow("Difficulty", ui_difficultyBox);

    QWidget *estimationWidget = new QWidget(this);
    QHBoxLayout *estimationLayout = new QHBoxLayout(estimationWidget);
    estimationLayout->setContentsMargins(0,0,0,0);
    estimationLayout->setSpacing(3);

    ui_estimationEdit = new AutoSelectDoubleSpinBox(this);
    ui_estimationEdit->setMinimum(-1);
    ui_estimationEdit->setMaximum(999);
    ui_estimationEdit->setSuffix(" days");
    ui_estimationEdit->setEnabled(false);
    estimationLayout->addWidget( ui_estimationEdit );

    ui_autoEstimationBox = new QCheckBox("Auto", this);
    ui_autoEstimationBox->setChecked(true);
    estimationLayout->addWidget( ui_autoEstimationBox );

    estimationLayout->setStretch(0,1);
    estimationLayout->setStretch(1,0);
    detailsLayout->addRow("Estimation", estimationWidget);

    ui_timeSpent = new AutoSelectSpinBox(this);
    ui_timeSpent->setMinimum(0);
    ui_timeSpent->setMaximum(9999);
    ui_timeSpent->setSuffix(" hours (0 days)");
    detailsLayout->addRow("Time spent", ui_timeSpent);

    ui_versionBox = new AutoSelectSpinBox(this);
    ui_versionBox->setMaximum(1000);
    ui_versionBox->setValue(1);
    ui_versionBox->setPrefix("v");
    detailsLayout->addRow("Version", ui_versionBox);

    ui_publishedBox = new QCheckBox("Published",this);
    detailsLayout->addRow("Publication", ui_publishedBox);

    ui_userBox = new RamObjectListComboBox(true, this);
    detailsLayout->addRow("Assigned user", ui_userBox);

    ui_mainLayout->addLayout( detailsLayout);

    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    buttonsLayout->setSpacing(3);
    buttonsLayout->setContentsMargins(0,0,0,0);

    buttonsLayout->addStretch();

    ui_revertButton = new QToolButton(this);
    ui_revertButton->setText("Revert");
    ui_revertButton->setToolTip("Cancel changes\nRevert to the current state.");
    ui_revertButton->setStatusTip("Revert to the current state.");
    ui_revertButton->setIcon(QIcon(":/icons/undo"));
    buttonsLayout->addWidget(ui_revertButton);

    /*ui_setButton = new QToolButton(this);
    ui_setButton->setText("Update");
    ui_setButton->setToolTip("Update / Save changes\nSave the changes for the current status.");
    ui_setButton->setStatusTip("Save the changes for the current status.");
    ui_setButton->setIcon(QIcon(":/icons/apply"));
    buttonsLayout->addWidget(ui_setButton);*/

    bottomLayout->addLayout(buttonsLayout);

    QTabWidget *tabWidget = new QTabWidget(this);

    QWidget *mainFilesWidget = new QWidget(tabWidget);
    QVBoxLayout *mainFileLayout = new QVBoxLayout(mainFilesWidget);
    mainFileLayout->setContentsMargins(0,0,0,0);
    mainFileLayout->setSpacing(3);

    ui_mainFileList = new QListWidget(this);
    mainFileLayout->addWidget(ui_mainFileList);

    QHBoxLayout *mainFileButtonLayout = new QHBoxLayout();
    mainFileButtonLayout->setContentsMargins(0,0,0,0);
    mainFileButtonLayout->setSpacing(3);
    mainFileLayout->addLayout(mainFileButtonLayout);

    ui_versionFileBox = new QComboBox(this);
    ui_versionFileBox->setEnabled(false);
    mainFileButtonLayout->addWidget(ui_versionFileBox);

    ui_createMainFileButton = new QToolButton(this);
    ui_createMainFileButton->setText("Create");
    ui_createMainFileButton->setToolTip("Create\nCreate a new file from the corresponding step template.");
    ui_createMainFileButton->setStatusTip("Create a new file from template.");
    ui_createMainFileButton->setIcon(QIcon(":/icons/create"));
    ui_createMainFileButton->setEnabled(false);
    mainFileButtonLayout->addWidget(ui_createMainFileButton);

    ui_createFromTemplateMenu = new QMenu(this);
    ui_createMainFileButton->setMenu(ui_createFromTemplateMenu);
    ui_createMainFileButton->setPopupMode(QToolButton::InstantPopup);

    ui_createFromTemplateAction = new QAction("Create from default template", this);
    ui_createFromTemplateMenu->addAction( ui_createFromTemplateAction );
    ui_createFromTemplateMenu->addSeparator();

    ui_openMainFileButton = new QToolButton(this);
    ui_openMainFileButton->setText("Open");
    ui_openMainFileButton->setToolTip("Open\nOpen the file.");
    ui_openMainFileButton->setStatusTip("Open the file.");
    ui_openMainFileButton->setIcon(QIcon(":/icons/open"));
    ui_openMainFileButton->setEnabled(false);
    mainFileButtonLayout->addWidget(ui_openMainFileButton);

    tabWidget->addTab(mainFilesWidget, QIcon(":/icons/files"), "Work");

    QWidget *publishedFilesWidget = new QWidget(tabWidget);
    QVBoxLayout *publishedFileLayout = new QVBoxLayout(publishedFilesWidget);
    publishedFileLayout->setContentsMargins(0,0,0,0);
    publishedFileLayout->setSpacing(3);

    ui_publishedFileList = new QListWidget(this);
    publishedFileLayout->addWidget(ui_publishedFileList);

    QHBoxLayout *publishedFileButtonLayout = new QHBoxLayout();
    publishedFileButtonLayout->setContentsMargins(0,0,0,0);
    publishedFileButtonLayout->setSpacing(3);
    publishedFileLayout->addLayout(publishedFileButtonLayout);
    publishedFileButtonLayout->addStretch();

    ui_openPublishedFileButton = new QToolButton(this);
    ui_openPublishedFileButton->setText("Open");
    ui_openPublishedFileButton->setToolTip("Open\nOpen the published file.");
    ui_openPublishedFileButton->setStatusTip("Open the file.");
    ui_openPublishedFileButton->setIcon(QIcon(":/icons/open"));
    ui_openPublishedFileButton->setEnabled(false);
    publishedFileButtonLayout->addWidget(ui_openPublishedFileButton);

    tabWidget->addTab(publishedFilesWidget, QIcon(":/icons/files"), "Published");

    QWidget *previewFilesWidget = new QWidget(tabWidget);
    QVBoxLayout *previewFileLayout = new QVBoxLayout(previewFilesWidget);
    previewFileLayout->setContentsMargins(0,0,0,0);
    previewFileLayout->setSpacing(3);

    ui_previewFileList = new QListWidget(this);
    previewFileLayout->addWidget(ui_previewFileList);

    QHBoxLayout *previewFileButtonLayout = new QHBoxLayout();
    previewFileButtonLayout->setContentsMargins(0,0,0,0);
    previewFileButtonLayout->setSpacing(3);
    previewFileLayout->addLayout(previewFileButtonLayout);
    previewFileButtonLayout->addStretch();

    ui_openPreviewFileButton = new QToolButton(this);
    ui_openPreviewFileButton->setText("Open");
    ui_openPreviewFileButton->setToolTip("Open\nOpen the preview file.");
    ui_openPreviewFileButton->setStatusTip("Open the file.");
    ui_openPreviewFileButton->setIcon(QIcon(":/icons/open"));
    ui_openPreviewFileButton->setEnabled(false);
    previewFileButtonLayout->addWidget(ui_openPreviewFileButton);

    tabWidget->addTab(previewFilesWidget, QIcon(":/icons/files"), "Preview");

    bottomLayout->addWidget(tabWidget);

    ui_folderWidget = new DuQFFolderDisplayWidget(this);
    bottomLayout->addWidget(ui_folderWidget);

    commentSplitter->addWidget(bottomWidget);
}

void StatusEditWidget::connectEvents()
{
    connect( ui_stateBox, SIGNAL(currentIndexChanged(int)), this, SLOT(update()));
    connect( ui_completionBox, SIGNAL(valueChanging(int)), this, SLOT(update()));
    connect( ui_versionBox, SIGNAL(valueChanged(int)), this, SLOT(update()));
    ui_statusCommentEdit->installEventFilter(this);
    connect( ui_userBox, SIGNAL(currentIndexChanged(int)), this, SLOT(update()));
    connect( ui_publishedBox, SIGNAL(clicked(bool)), this, SLOT(update()));
    connect( ui_autoEstimationBox, SIGNAL(clicked(bool)), this, SLOT(update()));
    connect( ui_timeSpent, SIGNAL(valueChanged(bool)), this, SLOT(update()));
    connect( ui_estimationEdit, SIGNAL(valueChanged(double)), this, SLOT(update()));
    connect( ui_difficultyBox, SIGNAL(currentIndexChanged(int)), this, SLOT(update()));

    connect(ui_stateBox, SIGNAL(currentObjectChanged(RamObject*)), this, SLOT(currentStateChanged(RamObject*)));
    connect(ui_revertButton, &QToolButton::clicked, this, &StatusEditWidget::revert);
    connect(ui_versionBox, SIGNAL(valueChanged(int)), this, SLOT(checkPublished(int)));

    connect(ui_mainFileList, SIGNAL(currentRowChanged(int)),this, SLOT(mainFileSelected(int)));
    connect(ui_mainFileList,SIGNAL(itemDoubleClicked(QListWidgetItem*)),this,SLOT(openMainFile()));
    connect(ui_openMainFileButton, SIGNAL(clicked()),this,SLOT(openMainFile()));
    connect(ui_createFromTemplateAction, SIGNAL(triggered()), this, SLOT(createFromDefaultTemplate()));

    connect(ui_publishedFileList, SIGNAL(currentRowChanged(int)),this, SLOT(publishedFileSelected(int)));
    connect(ui_previewFileList, SIGNAL(currentRowChanged(int)),this, SLOT(previewFileSelected(int)));
    connect(ui_openPublishedFileButton, SIGNAL(clicked()),this,SLOT(openPublishedFile()));
    connect(ui_openPreviewFileButton, SIGNAL(clicked()),this,SLOT(openPreviewFile()));
    connect(ui_publishedFileList,SIGNAL(itemDoubleClicked(QListWidgetItem*)),this,SLOT(openPublishedFile()));
    connect(ui_previewFileList,SIGNAL(itemDoubleClicked(QListWidgetItem*)),this,SLOT(openPreviewFile()));

    connect(ui_autoEstimationBox, SIGNAL(toggled(bool)), this, SLOT(autoEstimate(bool)));
    connect(ui_difficultyBox, SIGNAL(currentIndexChanged(int)), this, SLOT(autoEstimate()));
    connect(ui_timeSpent, SIGNAL(valueChanged(int)),this, SLOT(estimateDays(int)));

    // Shortcuts
    QShortcut *s;
    s = new QShortcut(QKeySequence(QKeySequence::Delete), ui_mainFileList, nullptr, nullptr, Qt::WidgetWithChildrenShortcut );
    connect( s, SIGNAL(activated()), this, SLOT(removeSelectedMainFile()) );
    s = new QShortcut(QKeySequence(QKeySequence::Delete), ui_publishedFileList, nullptr, nullptr, Qt::WidgetWithChildrenShortcut );
    connect( s, SIGNAL(activated()), this, SLOT(removeSelectedPublishedFile()) );
    s = new QShortcut(QKeySequence(QKeySequence::Delete), ui_previewFileList, nullptr, nullptr, Qt::WidgetWithChildrenShortcut );
    connect( s, SIGNAL(activated()), this, SLOT(removeSelectedPreviewFile()) );

    monitorDbQuery("updateStatus");
}
