#include "itemtablemanagerwidget.h"

ItemTableManagerWidget::ItemTableManagerWidget(RamStep::Type productionType, QWidget *parent) : QWidget(parent)
{   
    m_productionType = productionType;
    setupUi();

    currentUserChanged(nullptr);
    filter(nullptr);

    connectEvents();
}

void ItemTableManagerWidget::selectAllSteps()
{
    QList<QAction*> actions = ui_stepMenu->actions();
    for (int i = 4; i < actions.count(); i++)
    {
        actions[i]->setChecked(true);
    }
}

void ItemTableManagerWidget::selectUserSteps()
{
    QList<QAction*> actions = ui_stepMenu->actions();
    RamUser *u = Ramses::instance()->currentUser();

    for (int i = 4; i < actions.count(); i++)
    {
        RamStep *step = reinterpret_cast<RamStep*>( actions[i]->data().toULongLong() );
        if (!step) continue;
        if (u->isStepAssigned(step))
            actions[i]->setChecked(true);
        else
            actions[i]->setChecked(false);
    }
}

void ItemTableManagerWidget::deselectSteps()
{
    QList<QAction*> actions = ui_stepMenu->actions();
    for (int i = 4; i < actions.count(); i++)
    {
        actions[i]->setChecked(false);
    }
}

void ItemTableManagerWidget::selectAllUsers()
{
    QList<QAction*> actions = ui_userMenu->actions();
    for (int i = 4; i < actions.count(); i++)
    {
        actions[i]->setChecked(true);
    }
}

void ItemTableManagerWidget::selectMyself()
{
    QList<QAction*> actions = ui_userMenu->actions();
    RamUser *u = Ramses::instance()->currentUser();

    ui_actionNotAssigned->setChecked(false);

    for (int i = 4; i < actions.count(); i++)
    {
        RamUser *user = reinterpret_cast<RamUser*>( actions[i]->data().toULongLong() );
        if (!user) continue;
        if (user->is(u))
            actions[i]->setChecked(true);
        else
            actions[i]->setChecked(false);
    }
}

void ItemTableManagerWidget::deselectUsers()
{
    QList<QAction*> actions = ui_userMenu->actions();
    for (int i = 4; i < actions.count(); i++)
    {
        actions[i]->setChecked(false);
    }
}

void ItemTableManagerWidget::showUnassigned(bool show)
{
    ui_table->filteredList()->showUnassigned(show);
    if (show) ui_table->resizeRowsToContents();
    checkUserFilters();
}

void ItemTableManagerWidget::selectAllStates()
{
    QList<QAction*> actions = ui_stateMenu->actions();
    for (int i = 3; i < actions.count(); i++)
    {
        actions[i]->setChecked(true);
    }
}

void ItemTableManagerWidget::deselectStates()
{
    QList<QAction*> actions = ui_stateMenu->actions();
    for (int i = 3; i < actions.count(); i++)
    {
        actions[i]->setChecked(false);
    }
}

void ItemTableManagerWidget::showEvent(QShowEvent *event)
{
    if (!event->spontaneous()) ui_titleBar->show();
    QWidget::showEvent(event);
}

void ItemTableManagerWidget::hideEvent(QHideEvent *event)
{
    if (!event->spontaneous())
    {
        ui_titleBar->hide();
    }

    // Save filters and layout
    RamUser *user = Ramses::instance()->currentUser();
    if (user)
    {
        QSettings *uSettings = user->settings();

        if (m_productionType == RamStep::ShotProduction)
            uSettings->beginGroup("shotTable");
        else
            uSettings->beginGroup("assetTable");

        // View
        //uSettings->setValue("showTimeTracking", ui_actionTimeTracking->isChecked());
        //uSettings->setValue("showCompletion", ui_actionCompletionRatio->isChecked());
        uSettings->setValue("showDetails", ui_actionShowDetails->isChecked());
        // Users
        ui_userMenu->saveState(uSettings, "users");
        // Steps
        ui_stepMenu->saveState(uSettings, "steps");
        // States
        ui_stateMenu->saveState(uSettings, "states");

        uSettings->endGroup();
    }
    QWidget::hideEvent(event);
}

void ItemTableManagerWidget::projectChanged(RamProject *project)
{
    this->setEnabled(false );

    if (!m_project && !project) return;
    if (m_project) if (m_project->is(project) ) return;

    while (!m_projectConnections.isEmpty()) disconnect( m_projectConnections.takeLast() );

    // Clear step list
    QList<QAction*> actions = ui_stepMenu->actions();
    for (int i = actions.count() -1; i >= 4; i--)
    {
        actions.at(i)->deleteLater();
    }
    ui_table->filteredList()->showAllSteps();

    // Clear user list
    actions = ui_userMenu->actions();
    for (int i = actions.count() -1; i >= 5; i--)
    {
        actions.at(i)->deleteLater();
    }
    ui_table->filteredList()->clearUsers();

    m_project = project;

    if(!project)
    {
        ui_table->setList(nullptr);
        return;
    }

    // Populate list and table
    ui_userMenu->setList( project->users() );
    ui_stepMenu->setList( project->steps() );
    ui_actionNotAssigned->setChecked(true);

    if (m_productionType == RamStep::AssetProduction)
    {
        ui_table->setList( project->assets() );
        ui_groupBox->setList( project->assetGroups() );
    }
    else if (m_productionType == RamStep::ShotProduction)
    {
        ui_table->setList( project->shots() );
        ui_groupBox->setList( project->sequences() );
    }

    ui_assignUserMenu->setList(project->users());
    ui_assignUserContextMenu->setList(project->users());

    /*ui_stateMenu->selectAll();
    ui_stepMenu->selectAll();
    ui_userMenu->selectAll();*/

    loadSettings();

    this->setEnabled(true);
}

void ItemTableManagerWidget::showUser(RamObject *userObj, bool s)
{
    if(!m_project) return;
    RamUser *user = qobject_cast<RamUser*>( userObj );
    if(s)
    {
        ui_table->filteredList()->showUser(user);
        //ui_table->resizeRowsToContents();
    }
    else
    {
        ui_table->filteredList()->hideUser(user);
    }
    checkUserFilters();
}

void ItemTableManagerWidget::showStep(RamObject *stepObj, bool s)
{
    if(!m_project) return;
    RamStep *step = qobject_cast<RamStep*>( stepObj );
    if(s)
    {
        ui_table->filteredList()->showStep( step );
        ui_table->resizeColumnsToContents();
        //ui_table->resizeRowsToContents();
    }
    else
    {
        ui_table->filteredList()->hideStep( step );
    }
    checkStepFilters();
}

void ItemTableManagerWidget::showState(RamObject *stateObj, bool s)
{
    if(!m_project) return;
    RamState *state = qobject_cast<RamState*>( stateObj );
    if(s)
    {
        ui_table->filteredList()->showState(state);
        ui_table->resizeRowsToContents();
    }
    else
    {
        ui_table->filteredList()->hideState(state);
    }
    checkStateFilters();
}

void ItemTableManagerWidget::checkStepFilters()
{
    bool ok = ui_stepMenu->isAllChecked();

    QString t = ui_stepButton->text().replace(" ▽","");
    if (!ok) t = t + " ▽";
    ui_stepButton->setText( t );
}

void ItemTableManagerWidget::checkUserFilters()
{
    bool ok = ui_userMenu->isAllChecked();

    QString t = ui_userButton->text().replace(" ▽","");
    if (!ok) t = t + " ▽";
    ui_userButton->setText( t );
}

void ItemTableManagerWidget::checkStateFilters()
{
    bool ok = ui_stateMenu->isAllChecked();

    QString t = ui_stateButton->text().replace(" ▽","");
    if (!ok) t = t + " ▽";
    ui_stateButton->setText( t );
}

void ItemTableManagerWidget::editObject(RamObject *obj) const
{
    if (!obj) return;
    // Check if it's a status
    if (obj->objectType() == RamObject::Status)
    {
        RamStatus *status = qobject_cast<RamStatus*>( obj );

        // If it's not the current user, create a new one
        RamUser *currentUser = Ramses::instance()->currentUser();
        if(!status->user()->is(currentUser))
            status = RamStatus::copy( status, currentUser );

        status->edit();
        return;
    }
    obj->edit();
}

void ItemTableManagerWidget::historyObject(RamObject *obj) const
{
    // get the step history
    RamStatus *status = qobject_cast<RamStatus*>(obj);
    RamStep *step = status->step();
    RamItem *item = status->item();
    item->statusHistory(step)->edit();
}

void ItemTableManagerWidget::filter(RamObject *filterObj)
{
    ui_table->filter(filterObj);

    if (m_productionType == RamStep::ShotProduction)
    {
        int shotCount = 0;
        double duration = 0;
        if (filterObj)
        {
            RamSequence *sequence = qobject_cast<RamSequence*>(filterObj);
            if (sequence)
            {
                shotCount = sequence->shotCount();
                duration = sequence->duration();
            }
        }
        else if (m_project)
        {
            shotCount = m_project->shots()->count();
            duration = m_project->duration();
        }
        QString title;
        if (shotCount == 1) title = QString::number(shotCount) + " Shot";
        else if (shotCount > 0) title = QString::number(shotCount) + " Shots";
        else title = "Shots";
        if (duration > 0) title += " (" + MediaUtils::durationToTimecode(duration) + ")";
        ui_titleBar->setTitle(title);
    }
    else
    {
        int assetCount = 0;
        if (filterObj)
        {
            RamAssetGroup *group = qobject_cast<RamAssetGroup*>(filterObj);
            if (group) assetCount = group->assetCount();
        }
        else if (m_project)
        {
            assetCount = m_project->assets()->count();
        }
        if (assetCount == 1) ui_titleBar->setTitle(QString::number(assetCount) + " Asset");
        else if (assetCount > 0) ui_titleBar->setTitle(QString::number(assetCount) + " Assets");
        else ui_titleBar->setTitle("Assets");
    }
}

void ItemTableManagerWidget::uncheckSort()
{
    ui_actionSortDefault->setChecked(false);
    ui_actionSortByShortName->setChecked(false);
    ui_actionSortByName->setChecked(false);
    ui_actionSortByDifficulty->setChecked(false);
    ui_actionSortByTimeSpent->setChecked(false);
    ui_actionSortByEstimation->setChecked(false);
    ui_actionSortByCompletion->setChecked(false);
}

void ItemTableManagerWidget::sortDefault(bool sort)
{
    Q_UNUSED(sort)
    uncheckSort();
    ui_actionSortDefault->setChecked(true);
    ui_header->setSortable(false);
    ui_table->filteredList()->setSortMode(RamItemFilterModel::Default);
}

void ItemTableManagerWidget::sortByShortName(bool sort)
{
    uncheckSort();
    ui_actionSortByShortName->setChecked(sort);
    ui_header->setSortable(sort);
    ui_actionSortDefault->setChecked( !sort );
    ui_table->filteredList()->setSortMode(RamItemFilterModel::ShortName);
}

void ItemTableManagerWidget::sortByName(bool sort)
{
    uncheckSort();
    ui_actionSortByName->setChecked(sort);
    ui_header->setSortable(sort);
    ui_actionSortDefault->setChecked( !sort );
    ui_table->filteredList()->setSortMode(RamItemFilterModel::Name);
}

void ItemTableManagerWidget::sortByDifficulty(bool sort)
{
    uncheckSort();
    ui_actionSortByDifficulty->setChecked(sort);
    ui_header->setSortable(sort);
    ui_actionSortDefault->setChecked( !sort );
    ui_table->filteredList()->setSortMode(RamItemFilterModel::Difficulty);
}

void ItemTableManagerWidget::sortByTimeSpent(bool sort)
{
    uncheckSort();
    ui_actionSortByTimeSpent->setChecked(sort);
    ui_header->setSortable(sort);
    ui_actionSortDefault->setChecked( !sort );
    ui_table->filteredList()->setSortMode(RamItemFilterModel::TimeSpent);
}

void ItemTableManagerWidget::sortByEstimation(bool sort)
{
    uncheckSort();
    ui_actionSortByEstimation->setChecked(sort);
    ui_header->setSortable(sort);
    ui_actionSortDefault->setChecked( !sort );
    ui_table->filteredList()->setSortMode(RamItemFilterModel::Estimation);
}

void ItemTableManagerWidget::sortByCompletion(bool sort)
{
    uncheckSort();
    ui_actionSortByCompletion->setChecked(sort);
    ui_header->setSortable(sort);
    ui_actionSortDefault->setChecked( !sort );
    ui_table->filteredList()->setSortMode(RamItemFilterModel::Completion);
}

void ItemTableManagerWidget::unassignUser()
{
    QList<RamStatus*> status = beginEditSelectedStatus();
    for (int i = 0; i < status.count(); i++)
    {
        status.at(i)->assignUser(nullptr);
        status.at(i)->update();
    }
}

void ItemTableManagerWidget::assignUser(RamObject *usrObj)
{
    RamUser *user = qobject_cast<RamUser*>( usrObj );
    if (!user) return;

    QList<RamStatus*> status = beginEditSelectedStatus();
    for (int i = 0; i < status.count(); i++)
    {
        status.at(i)->assignUser(user);
        status.at(i)->update();
    }
}

void ItemTableManagerWidget::changeState(RamObject *sttObj)
{
    RamState *stt = qobject_cast<RamState*>( sttObj );
    if (!stt) return;

    QList<RamStatus*> status = beginEditSelectedStatus();
    for (int i = 0; i < status.count(); i++)
    {
        status.at(i)->setState(stt);
        status.at(i)->update();
    }
}

void ItemTableManagerWidget::setVeryEasy()
{
    setDiffculty(RamStatus::VeryEasy);
}

void ItemTableManagerWidget::setEasy()
{
    setDiffculty(RamStatus::Easy);
}

void ItemTableManagerWidget::setMedium()
{
    setDiffculty(RamStatus::Medium);
}

void ItemTableManagerWidget::setHard()
{
    setDiffculty(RamStatus::Hard);
}

void ItemTableManagerWidget::setVeryHard()
{
    setDiffculty(RamStatus::VeryHard);
}

void ItemTableManagerWidget::setDiffculty(RamStatus::Difficulty difficulty)
{
    QList<RamStatus*> status = beginEditSelectedStatus();
    for (int i = 0; i < status.count(); i++)
    {
        status.at(i)->setDifficulty( difficulty );
        status.at(i)->update();
    }
}

void ItemTableManagerWidget::setCompletion()
{
    QAction* action = qobject_cast<QAction*>( sender() );
    int completion = action->data().toInt();
    QList<RamStatus*> status = beginEditSelectedStatus();
    for (int i = 0; i < status.count(); i++)
    {
        status.at(i)->setCompletionRatio( completion );
        status.at(i)->update();
    }
}

void ItemTableManagerWidget::copyComment()
{
    QModelIndex currentIndex = ui_table->selectionModel()->currentIndex();
    if ( !currentIndex.isValid() ) return;

    const quintptr &iptr = currentIndex.data(Qt::UserRole).toULongLong();
    if (iptr == 0) return;
    RamObject *obj = reinterpret_cast<RamObject*>( iptr );
    if (obj->objectType() != RamObject::Status) return;
    RamStatus *status = qobject_cast<RamStatus*>(obj);
    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setText( status->comment() );
}

void ItemTableManagerWidget::cutComment()
{
    QModelIndex currentIndex = ui_table->selectionModel()->currentIndex();
    if ( !currentIndex.isValid() ) return;

    const quintptr &iptr = currentIndex.data(Qt::UserRole).toULongLong();
    if (iptr == 0) return;
    RamObject *obj = reinterpret_cast<RamObject*>( iptr );
    if (obj->objectType() != RamObject::Status) return;
    RamStatus *status = qobject_cast<RamStatus*>(obj);

    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setText( status->comment() );

    // If it's not the current user, create a new one
    RamUser *currentUser = Ramses::instance()->currentUser();
    if(!status->user()->is(currentUser))
        status = RamStatus::copy( status, currentUser );

    status->setComment("");
    status->update();
}

void ItemTableManagerWidget::pasteComment()
{
    QClipboard *clipboard = QGuiApplication::clipboard();
    QString comment = clipboard->text();
    if (comment == "") return;

    QList<RamStatus*> status = beginEditSelectedStatus();
    for (int i = 0; i < status.count(); i++)
    {
        status.at(i)->setComment( comment );
        status.at(i)->update();
    }
}

void ItemTableManagerWidget::createItem()
{
    RamProject *project = Ramses::instance()->currentProject();
    if (!project) return;
    RamObject *group = ui_groupBox->currentObject();

    if(m_productionType == RamStep::ShotProduction)
    {
        if (project->sequences()->count() == 0 ) return;
        RamSequence *seq;
        if (!group) seq = qobject_cast<RamSequence*>( project->sequences()->at(0) );
        else seq = qobject_cast<RamSequence*>( group );
        if(!seq) return;

        RamShot *shot = new RamShot(
                    "NEW",
                    seq,
                    "New Shot"
                    );

        project->shots()->append(shot);
        editObject(shot);
    }
    else
    {
        if (project->assetGroups()->count() == 0 ) return;
        RamAssetGroup *ag;
        if (!group) ag = qobject_cast<RamAssetGroup*>( project->assetGroups()->at(0) );
        else ag = qobject_cast<RamAssetGroup*>( group );
        if(!ag) return;

        RamAsset *asset = new RamAsset(
                    "NEW",
                    ag,
                    "New Asset"
                    );

        project->assets()->append(asset);
        editObject(asset);
    }

}

void ItemTableManagerWidget::deleteItems()
{
    RamProject *project = Ramses::instance()->currentProject();
    if (!project) return;

    QModelIndexList selection = ui_table->selectionModel()->selectedIndexes();
    QModelIndexList selectedItems;

    for (int i = 0; i < selection.count(); i++)
    {
        // remove only if it's the item in the first column
        if (selection.at(i).column() != 0) continue;
        int row = selection.at(i).row();
        if (m_productionType == RamStep::ShotProduction)
            selectedItems << project->shots()->index( row, 0);
        else
            selectedItems << project->assets()->index( row, 0);
    }

    if (selectedItems.count() == 0) return;

    QMessageBox::StandardButton confirm = QMessageBox::question( this,
        "Confirm deletion",
        "Are you sure you want to premanently remove the selected items?" );

    if ( confirm != QMessageBox::Yes) return;

    QList<RamObject*> objs;

    if (m_productionType == RamStep::ShotProduction) objs = project->shots()->removeIndices(selectedItems);
    else objs = project->assets()->removeIndices(selectedItems);

    for (int i = objs.count() -1 ; i >= 0; i--)
    {
        objs.at(i)->remove();
    }
}

void ItemTableManagerWidget::createMultiple()
{
    RamProject *project = Ramses::instance()->currentProject();
    if (!project) return;

    ShotsCreationDialog dialog(project, this);
    dialog.exec();
}

void ItemTableManagerWidget::contextMenuRequested(QPoint p)
{
    // Call the context menu
    ui_contextMenu->popup(ui_table->viewport()->mapToGlobal(p));
}

void ItemTableManagerWidget::currentUserChanged(RamUser *user)
{

    ui_actionItem->setVisible(false);
    if (!user) return;
    if (user->role() >= RamUser::ProjectAdmin)
    {
        ui_actionItem->setVisible(true);
    }
    loadSettings();
}

void ItemTableManagerWidget::setupUi()
{
    // Get the mainwindow to add the titlebar
    QMainWindow *mw = GuiUtils::appMainWindow();
    mw->addToolBarBreak(Qt::TopToolBarArea);

    ui_titleBar = new DuQFTitleBar("Assets",false, mw);
    ui_titleBar->setObjectName("assetToolBar");
    ui_titleBar->showReinitButton(false);
    mw->addToolBar(Qt::TopToolBarArea,ui_titleBar);
    ui_titleBar->setFloatable(false);
    ui_titleBar->hide();

    // group box
    ui_groupBox = new RamObjectListComboBox(true,this);
    ui_titleBar->insertLeft(ui_groupBox);

    // Search field
    ui_searchEdit = new DuQFSearchEdit(this);
    ui_searchEdit->setMaximumWidth(150);
    ui_searchEdit->hideSearchButton();
    ui_titleBar->insertLeft(ui_searchEdit);

    // View Menu
    QMenu *viewMenu = new QMenu(this);

    /*ui_actionTimeTracking = new QAction("Show time tracking", this);
    ui_actionTimeTracking->setCheckable(true);
    ui_actionTimeTracking->setChecked(true);
    viewMenu->addAction(ui_actionTimeTracking);//*/

    /*ui_actionCompletionRatio = new QAction("Show completion", this);
    ui_actionCompletionRatio->setCheckable(true);
    ui_actionCompletionRatio->setChecked(true);
    viewMenu->addAction(ui_actionCompletionRatio);//*/

    ui_actionShowDetails = new QAction("Show details", this);
    ui_actionShowDetails->setCheckable(true);
    viewMenu->addAction(ui_actionShowDetails);

    QToolButton *viewButton = new QToolButton(this);
    viewButton->setText(" View");
    viewButton->setIcon(QIcon(":/icons/show"));
    viewButton->setMenu(viewMenu);
    viewButton->setIconSize(QSize(16,16));
    viewButton->setObjectName("menuButton");
    viewButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    viewButton->setPopupMode(QToolButton::InstantPopup);
    ui_titleBar->insertLeft(viewButton);

    // Sort Menu
    QMenu *sortMenu = new QMenu(this);

    ui_actionSortDefault = new QAction("Default", this);
    ui_actionSortDefault->setCheckable(true);
    ui_actionSortDefault->setChecked(true);
    sortMenu->addAction(ui_actionSortDefault);

    ui_actionSortByShortName = new QAction("By ID", this);
    ui_actionSortByShortName->setCheckable(true);
    sortMenu->addAction(ui_actionSortByShortName);

    ui_actionSortByName = new QAction("By name", this);
    ui_actionSortByName->setCheckable(true);
    sortMenu->addAction(ui_actionSortByName);

    ui_actionSortByDifficulty = new QAction("By difficulty", this);
    ui_actionSortByDifficulty->setCheckable(true);
    sortMenu->addAction(ui_actionSortByDifficulty);

    ui_actionSortByTimeSpent = new QAction("By time spent", this);
    ui_actionSortByTimeSpent->setCheckable(true);
    sortMenu->addAction(ui_actionSortByTimeSpent);

    ui_actionSortByEstimation = new QAction("By estimation / Goal", this);
    ui_actionSortByEstimation->setCheckable(true);
    sortMenu->addAction(ui_actionSortByEstimation);

    ui_actionSortByCompletion = new QAction("By completion ratio", this);
    ui_actionSortByCompletion->setCheckable(true);
    sortMenu->addAction(ui_actionSortByCompletion);

    QToolButton *sortButton = new QToolButton(this);
    sortButton->setText(" Sort");
    sortButton->setIcon(QIcon(":/icons/sort"));
    sortButton->setMenu(sortMenu);
    sortButton->setIconSize(QSize(16,16));
    sortButton->setObjectName("menuButton");
    sortButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    sortButton->setPopupMode(QToolButton::InstantPopup);
    ui_titleBar->insertLeft(sortButton);

    // Item Menu
    ui_itemMenu = new QMenu(this);

    QString createItemLabel;
    QString deleteItemLabel;
    if (m_productionType == RamStep::ShotProduction)
    {
        createItemLabel = "Create new shot";
        deleteItemLabel = "Remove selected shots";
    }
    else
    {
        createItemLabel = "Create new asset";
        deleteItemLabel = "Remove selected assets";
    }

    ui_actionCreateItem = new QAction(QIcon(":/icons/add"), createItemLabel, this);
    ui_itemMenu->addAction(ui_actionCreateItem);

    ui_actionDeleteItem = new QAction(QIcon(":/icons/remove"), deleteItemLabel, this);
    ui_itemMenu->addAction(ui_actionDeleteItem);

    ui_actionCreateMultiple = new QAction("Create multiple shots...", this);

    if (m_productionType == RamStep::ShotProduction)
    {
        ui_itemMenu->addAction(ui_actionCreateMultiple);
    }

    ui_itemButton = new QToolButton(this);
    if (m_productionType == RamStep::ShotProduction) ui_itemButton->setText(" Shots");
    else ui_itemButton->setText(" Assets");
    if (m_productionType == RamStep::ShotProduction) ui_itemButton->setIcon(QIcon(":/icons/shot"));
    else ui_itemButton->setIcon(QIcon(":/icons/asset"));
    ui_itemButton->setMenu(ui_itemMenu);
    ui_itemButton->setIconSize(QSize(16,16));
    ui_itemButton->setObjectName("menuButton");
    ui_itemButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    ui_itemButton->setPopupMode(QToolButton::InstantPopup);
    ui_actionItem = ui_titleBar->insertLeft(ui_itemButton);

    // User Menu
    ui_userMenu = new RamObjectListMenu(true, this);

    ui_actionSelectMyself = new QAction("Select myself", this);
    ui_userMenu->insertAction( ui_userMenu->actions().at(0), ui_actionSelectMyself);

    ui_actionNotAssigned = new QAction("Unassigned", this);
    ui_actionNotAssigned->setCheckable(true);
    ui_actionNotAssigned->setChecked(true);
    ui_userMenu->addAction(ui_actionNotAssigned);

    ui_userButton = new QToolButton(this);
    ui_userButton->setText(" Users");
    ui_userButton->setIcon(QIcon(":/icons/user"));
    ui_userButton->setIconSize(QSize(16,16));
    ui_userButton->setObjectName("menuButton");
    ui_userButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    ui_userButton->setPopupMode(QToolButton::InstantPopup);
    ui_userButton->setMenu(ui_userMenu);

    ui_titleBar->insertLeft(ui_userButton);

    // Step Menu
    ui_stepMenu = new RamObjectListMenu(true, this);

    ui_actionSelectMySteps = new QAction("Select my steps", this);
    ui_stepMenu->insertAction( ui_stepMenu->actions().at(2), ui_actionSelectMySteps);

    ui_stepButton = new QToolButton(this);
    ui_stepButton->setText(" Steps");
    ui_stepButton->setIcon(QIcon(":/icons/step"));
    ui_stepButton->setIconSize(QSize(16,16));
    ui_stepButton->setObjectName("menuButton");
    ui_stepButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    ui_stepButton->setPopupMode(QToolButton::InstantPopup);
    ui_stepButton->setMenu(ui_stepMenu);

    ui_titleBar->insertLeft(ui_stepButton);

    // State menu

    ui_stateMenu = new RamObjectListMenu(true, this);

    ui_stateButton = new QToolButton(this);
    ui_stateButton->setText(" States");
    ui_stateButton->setIcon(QIcon(":/icons/state-l"));
    ui_stateButton->setIconSize(QSize(16,16));
    ui_stateButton->setObjectName("menuButton");
    ui_stateButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    ui_stateButton->setPopupMode(QToolButton::InstantPopup);
    ui_stateButton->setMenu(ui_stateMenu);

    ui_stateMenu->setList( Ramses::instance()->states() );

    ui_titleBar->insertLeft(ui_stateButton);

    // Status menu
    QMenu *statusMenu = new QMenu(this);

    ui_copyComment = new QAction("Copy comment", this);
    ui_copyComment->setShortcut(QKeySequence("Ctrl+C"));
    ui_copyComment->setIcon(QIcon(":/icons/copy"));
    statusMenu->addAction(ui_copyComment);

    ui_cutComment = new QAction("Cut comment", this);
    ui_cutComment->setShortcut(QKeySequence("Ctrl+X"));
    ui_cutComment->setIcon(QIcon(":/icons/cut"));
    statusMenu->addAction(ui_cutComment);

    ui_pasteComment = new QAction("Paste as comment", this);
    ui_pasteComment->setShortcut(QKeySequence("Ctrl+V"));
    ui_pasteComment->setIcon(QIcon(":/icons/paste"));
    statusMenu->addAction(ui_pasteComment);

    statusMenu->addSeparator();

    ui_assignUserMenu = new RamObjectListMenu(false, this);
    ui_assignUserMenu->setTitle("Assign user");
    ui_assignUserMenu->addCreateButton();
    ui_assignUserMenu->actions().at(0)->setText("None");
    statusMenu->addMenu(ui_assignUserMenu);

    ui_changeStateMenu = new RamObjectListMenu(false, this);
    ui_changeStateMenu->setTitle("Change state");
    ui_changeStateMenu->setList(Ramses::instance()->states());
    statusMenu->addMenu(ui_changeStateMenu);

    ui_changeDifficultyMenu = new QMenu("Change difficulty", this);
    ui_veryEasy = new QAction("Very easy", this);
    ui_easy = new QAction("Easy", this);
    ui_medium = new QAction("Medium", this);
    ui_hard = new QAction("Hard", this);
    ui_veryHard = new QAction("Very hard", this);
    ui_changeDifficultyMenu->addAction(ui_veryEasy);
    ui_changeDifficultyMenu->addAction(ui_easy);
    ui_changeDifficultyMenu->addAction(ui_medium);
    ui_changeDifficultyMenu->addAction(ui_hard);
    ui_changeDifficultyMenu->addAction(ui_veryHard);
    statusMenu->addMenu(ui_changeDifficultyMenu);

    QMenu *completionMenu = new QMenu("Set completion", this);
    ui_completion0   = new QAction("0%",this);
    ui_completion10  = new QAction("10%",this);
    ui_completion25  = new QAction("25%",this);
    ui_completion50  = new QAction("50%",this);
    ui_completion75  = new QAction("75%",this);
    ui_completion90  = new QAction("90%",this);
    ui_completion100 = new QAction("100%",this);
    ui_completion0->setData(0);
    ui_completion10->setData(10);
    ui_completion25->setData(25);
    ui_completion50->setData(50);
    ui_completion75->setData(75);
    ui_completion90->setData(90);
    ui_completion100->setData(100);
    completionMenu->addAction(ui_completion0  );
    completionMenu->addAction(ui_completion10 );
    completionMenu->addAction(ui_completion25 );
    completionMenu->addAction(ui_completion50 );
    completionMenu->addAction(ui_completion75 );
    completionMenu->addAction(ui_completion90 );
    completionMenu->addAction(ui_completion100);
    statusMenu->addMenu(completionMenu);


    QToolButton *statusButton = new QToolButton(this);
    statusButton->setText(" Status");
    statusButton->setIcon(QIcon(":/icons/status-edit"));
    statusButton->setIconSize(QSize(16,16));
    statusButton->setObjectName("menuButton");
    statusButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    statusButton->setPopupMode(QToolButton::InstantPopup);
    statusButton->setMenu(statusMenu);

    ui_titleBar->insertLeft(statusButton);


    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->setSpacing(3);
    mainLayout->setContentsMargins(0,0,0,0);

    ui_table = new RamObjectListView(RamObjectListView::Table, this);
    //ui_table->setEditableObjects(true, RamUser::ProjectAdmin);
    ui_table->setEditableObjects(false);
    ui_table->setContextMenuPolicy(Qt::CustomContextMenu);
    ui_header = new RamStepHeaderView(ui_table);
    ui_table->setHorizontalHeader( ui_header );
    ui_table->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui_table->setTimeTracking(false);
    ui_header->setTimeTracking(false);
    if (m_productionType == RamStep::ShotProduction) ui_table->setSortable(true);
    mainLayout->addWidget(ui_table);

    ui_table->filteredList()->setStepType(m_productionType);
    ui_table->filteredList()->useFilters(true);

    this->setLayout(mainLayout);

    ui_contextMenu = new QMenu(this);

    ui_contextMenu->addAction(ui_copyComment);
    ui_contextMenu->addAction(ui_cutComment);
    ui_contextMenu->addAction(ui_pasteComment);

    ui_contextMenu->addSeparator();

    ui_assignUserContextMenu = new RamObjectListMenu(false, this);
    ui_assignUserContextMenu->setTitle("Assign user");
    ui_assignUserContextMenu->addCreateButton();
    ui_assignUserContextMenu->actions().at(0)->setText("None");
    ui_contextMenu->addMenu(ui_assignUserMenu);

    ui_changeStateContextMenu = new RamObjectListMenu(false, this);
    ui_changeStateContextMenu->setTitle("Change state");
    ui_changeStateContextMenu->setList(Ramses::instance()->states());
    ui_contextMenu->addMenu(ui_changeStateContextMenu);

    QMenu *changeDifficultyContextMenu = new QMenu("Change difficulty", this);
    changeDifficultyContextMenu->addAction(ui_veryEasy);
    changeDifficultyContextMenu->addAction(ui_easy);
    changeDifficultyContextMenu->addAction(ui_medium);
    changeDifficultyContextMenu->addAction(ui_hard);
    changeDifficultyContextMenu->addAction(ui_veryHard);
    ui_contextMenu->addMenu(changeDifficultyContextMenu);

    QMenu *completionContextMenu = new QMenu("Set completion", this);
    completionContextMenu->addAction(ui_completion0  );
    completionContextMenu->addAction(ui_completion10 );
    completionContextMenu->addAction(ui_completion25 );
    completionContextMenu->addAction(ui_completion50 );
    completionContextMenu->addAction(ui_completion75 );
    completionContextMenu->addAction(ui_completion90 );
    completionContextMenu->addAction(ui_completion100);
    ui_contextMenu->addMenu(completionContextMenu);
}

void ItemTableManagerWidget::connectEvents()
{
    // Item actions
    connect(ui_actionCreateItem,SIGNAL(triggered()),this,SLOT(createItem()));
    connect(ui_actionDeleteItem,SIGNAL(triggered()),this,SLOT(deleteItems()));
    connect(ui_actionCreateMultiple,SIGNAL(triggered()),this,SLOT(createMultiple()));
    // Status actions
    connect(ui_assignUserMenu,SIGNAL(create()),this,SLOT(unassignUser()));
    connect(ui_assignUserMenu,SIGNAL(assign(RamObject*)),this,SLOT(assignUser(RamObject*)));
    connect(ui_changeStateMenu,SIGNAL(assign(RamObject*)),this,SLOT(changeState(RamObject*)));
    connect(ui_assignUserContextMenu,SIGNAL(create()),this,SLOT(unassignUser()));
    connect(ui_assignUserContextMenu,SIGNAL(assign(RamObject*)),this,SLOT(assignUser(RamObject*)));
    connect(ui_changeStateContextMenu,SIGNAL(assign(RamObject*)),this,SLOT(changeState(RamObject*)));
    connect(ui_veryEasy,SIGNAL(triggered()),this,SLOT(setVeryEasy()));
    connect(ui_easy,SIGNAL(triggered()),this,SLOT(setEasy()));
    connect(ui_medium,SIGNAL(triggered()),this,SLOT(setMedium()));
    connect(ui_hard,SIGNAL(triggered()),this,SLOT(setHard()));
    connect(ui_veryHard,SIGNAL(triggered()),this,SLOT(setVeryHard()));
    connect(ui_completion0  , SIGNAL(triggered()), this, SLOT( setCompletion() ) );
    connect(ui_completion10 , SIGNAL(triggered()), this, SLOT( setCompletion() ) );
    connect(ui_completion25 , SIGNAL(triggered()), this, SLOT( setCompletion() ) );
    connect(ui_completion50 , SIGNAL(triggered()), this, SLOT( setCompletion() ) );
    connect(ui_completion75 , SIGNAL(triggered()), this, SLOT( setCompletion() ) );
    connect(ui_completion90 , SIGNAL(triggered()), this, SLOT( setCompletion() ) );
    connect(ui_completion100, SIGNAL(triggered()), this, SLOT( setCompletion() ) );
    connect(ui_table, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenuRequested(QPoint)));
    // view actions
    //connect(ui_actionTimeTracking, SIGNAL(toggled(bool)), ui_table, SLOT(setTimeTracking(bool)));
    //connect(ui_actionCompletionRatio, SIGNAL(toggled(bool)), ui_table, SLOT(setCompletionRatio(bool)));
    //connect(ui_actionTimeTracking, SIGNAL(toggled(bool)), ui_header, SLOT(setTimeTracking(bool)));
    //connect(ui_actionCompletionRatio, SIGNAL(toggled(bool)), ui_header, SLOT(setCompletionRatio(bool)));
    connect(ui_actionShowDetails, SIGNAL(toggled(bool)), ui_table, SLOT(showDetails(bool)));
    // sort actions
    connect(ui_actionSortDefault, SIGNAL(triggered(bool)), this, SLOT(sortDefault(bool)));
    connect(ui_actionSortByShortName, SIGNAL(triggered(bool)), this, SLOT(sortByShortName(bool)));
    connect(ui_actionSortByName, SIGNAL(triggered(bool)), this, SLOT(sortByName(bool)));
    connect(ui_actionSortByDifficulty, SIGNAL(triggered(bool)), this, SLOT(sortByDifficulty(bool)));
    connect(ui_actionSortByTimeSpent, SIGNAL(triggered(bool)), this, SLOT(sortByTimeSpent(bool)));
    connect(ui_actionSortByEstimation, SIGNAL(triggered(bool)), this, SLOT(sortByEstimation(bool)));
    connect(ui_actionSortByCompletion, SIGNAL(triggered(bool)), this, SLOT(sortByCompletion(bool)));
    // step actions
    connect(ui_stepMenu,SIGNAL(assign(RamObject*,bool)), this, SLOT(showStep(RamObject*,bool)));
    connect(ui_actionSelectMySteps, SIGNAL(triggered()), this, SLOT(selectUserSteps()));
    // user actions
    connect(ui_userMenu,SIGNAL(assign(RamObject*,bool)), this, SLOT(showUser(RamObject*,bool)));
    connect(ui_actionNotAssigned, SIGNAL(toggled(bool)), this, SLOT(showUnassigned(bool)));
    connect(ui_actionSelectMyself, SIGNAL(triggered()), this, SLOT(selectMyself()));
    // state actions
    connect(ui_stateMenu,SIGNAL(assign(RamObject*,bool)), this, SLOT(showState(RamObject*,bool)));
    // comment actions
    connect(ui_copyComment, SIGNAL(triggered()), this, SLOT(copyComment()));
    connect(ui_cutComment, SIGNAL(triggered()), this, SLOT(cutComment()));
    connect(ui_pasteComment, SIGNAL(triggered()), this, SLOT(pasteComment()));
    // cell buttons
    connect(ui_table, SIGNAL(editObject(RamObject*)), this, SLOT(editObject(RamObject*)));
    connect(ui_table, SIGNAL(historyObject(RamObject*)), this, SLOT(historyObject(RamObject*)));
    // search
    connect(ui_searchEdit, SIGNAL(changing(QString)), ui_table, SLOT(search(QString)));
    connect(ui_searchEdit, SIGNAL(changed(QString)), ui_table, SLOT(search(QString)));
    // group filter
    connect(ui_groupBox, SIGNAL(currentObjectChanged(RamObject*)), this, SLOT(filter(RamObject*)));
    // other
    connect(ui_titleBar, &DuQFTitleBar::closeRequested, this, &ItemTableManagerWidget::closeRequested);
    connect(Ramses::instance(), &Ramses::currentProjectChanged, this, &ItemTableManagerWidget::projectChanged);
    connect(Ramses::instance(), SIGNAL(loggedIn(RamUser*)), this, SLOT(currentUserChanged(RamUser*)));
    connect(ui_header, SIGNAL(sort(int,Qt::SortOrder)), ui_table->filteredList(), SLOT(resort(int,Qt::SortOrder)));
    connect(ui_header, SIGNAL(unsort()), ui_table->filteredList(), SLOT(unsort()));
}

void ItemTableManagerWidget::loadSettings()
{
    // Freeze filters to improve performance
    ui_table->filteredList()->freeze();

    RamUser *u = Ramses::instance()->currentUser();
    if (!u) return;
    QSettings *uSettings = u->settings();
    if (m_productionType == RamStep::ShotProduction)
        uSettings->beginGroup("shotTable");
    else
        uSettings->beginGroup("assetTable");

    // View
    //ui_actionTimeTracking->setChecked( uSettings->value("showTimeTracking", true).toBool() );
    //ui_actionCompletionRatio->setChecked( uSettings->value("showCompletion", true).toBool() );
    ui_actionShowDetails->setChecked( uSettings->value("showDetails", true).toBool() );
    // Users
    ui_userMenu->restoreState(uSettings, "users");
    // Steps
    ui_stepMenu->restoreState(uSettings, "steps");
    // States
    ui_stateMenu->restoreState(uSettings, "states");

    uSettings->endGroup();

    // Unfreeze
    ui_table->filteredList()->unFreeze();
}

QList<RamStatus *> ItemTableManagerWidget::beginEditSelectedStatus()
{
    QList<RamStatus*> statuses;
    RamUser *currentUser = Ramses::instance()->currentUser();
    QModelIndexList selection = ui_table->selectionModel()->selectedIndexes();
    for (int i = 0; i < selection.count(); i++)
    {
        const QModelIndex &index = selection.at(i);
        const quintptr &iptr = index.data(Qt::UserRole).toULongLong();
        if (iptr == 0) continue;
        RamObject *obj = reinterpret_cast<RamObject*>( iptr );
        if (obj->objectType() != RamObject::Status) continue;
        RamStatus *status = qobject_cast<RamStatus*>(obj);
        if (!status->user()->is( currentUser ))
            status = RamStatus::copy( status, currentUser );
        statuses << status;
    }
    return statuses;
}
