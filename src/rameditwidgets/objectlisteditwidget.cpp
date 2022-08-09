#include "objectlisteditwidget.h"
#include "data-models/ramobjectfiltermodel.h"
#include "duqf-app/app-version.h"

#include "ramses.h"

template<typename RO, typename ROF>
ObjectListEditWidget<RO,ROF>::ObjectListEditWidget(bool editableObjects, RamUser::UserRole editRole, QWidget *parent) :
    QWidget(parent)
{
    m_objectList = nullptr;
    setupUi(editableObjects, editRole);
    connectEvents();
}

template<typename RO, typename ROF>
ObjectListEditWidget<RO,ROF>::ObjectListEditWidget(RamObjectList<RO> *objectList, bool editableObjects, RamUser::UserRole editRole, QWidget *parent) :
    QWidget(parent)
{
    setupUi(editableObjects, editRole);
    connectEvents();
    setList(objectList);
}

template<typename RO, typename ROF>
void ObjectListEditWidget<RO,ROF>::setList(RamObjectList<RO> *objectList)
{
    if (m_objectList) disconnect(ui_listWidget->filteredList(), nullptr, this, nullptr);

    // Show all
    ui_assignMenu->showAll();

    setFilterList(nullptr);
    m_objectList = objectList;
    ui_listWidget->setList(m_objectList);

    if (!objectList) return;

    // assignment
    connect(ui_listWidget->filteredList(), SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(objectAssigned(QModelIndex,int,int)));
    connect(ui_listWidget->filteredList(), SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)), this, SLOT(objectUnassigned(QModelIndex,int,int)));

    objectAssigned(QModelIndex(), 0, m_objectList->count() - 1);
}

template<typename RO, typename ROF>
void ObjectListEditWidget<RO,ROF>::setFilterList(RamObjectList<ROF> *filterList)
{
    ui_filterBox->setList(filterList);
    m_filterList = filterList;
    if (filterList)
    {
        ui_title->hide();
        ui_filterBox->show();
    }
    else
    {
        ui_title->show();
        ui_filterBox->hide();
    }
}


template<typename RO, typename ROF>
void ObjectListEditWidget<RO,ROF>::setAssignList(RamObjectList<RO> *assignList)
{
    ui_assignMenu->setList(assignList);
    ui_addButton->setPopupMode(QToolButton::InstantPopup);
    ui_addButton->setMenu(ui_assignMenu);

    // hide already assigned
    if(m_objectList)
    {
        if (m_objectList->count())
            objectAssigned(QModelIndex(), 0, m_objectList->count() - 1);
    }

    m_useAssignList = true;
}

template<typename RO, typename ROF>
void ObjectListEditWidget<RO,ROF>::setDontRemoveShortNameList(QStringList dontRemove)
{
    m_dontRemove = dontRemove;
}

template<typename RO, typename ROF>
void ObjectListEditWidget<RO,ROF>::clear()
{
    ui_listWidget->setList(nullptr);
}

template<typename RO, typename ROF>
void ObjectListEditWidget<RO,ROF>::setEditMode(EditMode editMode)
{
    m_editMode = editMode;
}

template<typename RO, typename ROF>
void ObjectListEditWidget<RO,ROF>::setEditable(bool editable)
{
    ui_removeButton->setVisible(editable);
    ui_addButton->setVisible(editable);
}

template<typename RO, typename ROF>
void ObjectListEditWidget<RO,ROF>::setSearchable(bool searchable)
{
    ui_searchEdit->setVisible(searchable);
}

template<typename RO, typename ROF>
void ObjectListEditWidget<RO,ROF>::setSortable(bool sortable)
{
    ui_listWidget->setSortable(sortable);
}

template<typename RO, typename ROF>
void ObjectListEditWidget<RO,ROF>::setTitle(QString title)
{
    ui_title->setVisible(title != "");
    ui_title->setText(title);
}

template<typename RO, typename ROF>
void ObjectListEditWidget<RO,ROF>::select(RO o)
{
    ui_listWidget->select(o);
}

template<typename RO, typename ROF>
void ObjectListEditWidget<RO,ROF>::setFilter(ROF o)
{
    QSignalBlocker b(ui_filterBox);
    ui_filterBox->setObject(o);
    ui_assignMenu->filter(o);
    ui_listWidget->filter(o);
}

template<typename RO, typename ROF>
QToolButton *ObjectListEditWidget<RO,ROF>::addButton() const
{
    return ui_addButton;
}

template<typename RO, typename ROF>
QString ObjectListEditWidget<RO,ROF>::currentFilterUuid() const
{
    return ui_filterBox->currentUuid();
}

template<typename RO, typename ROF>
ROF ObjectListEditWidget<RO,ROF>::currentFilter() const
{
    return ui_filterBox->currentObject();
}

template<typename RO, typename ROF>
RamObjectListView<RO> *ObjectListEditWidget<RO,ROF>::listWidget()
{
    return ui_listWidget;
}

template<typename RO, typename ROF>
void ObjectListEditWidget<RO,ROF>::removeSelectedObjects()
{
    QModelIndexList selection = ui_listWidget->selectionModel()->selectedRows();
    if (selection.count() == 0) return;

    if (m_editMode == RemoveObjects)
    {
        QMessageBox::StandardButton confirm = QMessageBox::question( this,
            "Confirm deletion",
            "Are you sure you want to premanently remove the selected items?" );

        if ( confirm != QMessageBox::Yes) return;
    }

    // Check if we can remove these objects
    for (int i = 0; i < selection.count(); i++)
    {
        QModelIndex index = selection.at(i);
        quintptr iptr = index.data(Qt::UserRole).toULongLong();
        if(iptr == 0) continue;
        RO o = reinterpret_cast<RO>( iptr );
        // Don't remove yourself if you're a user
        if (o->objectType() == RamObject::User)
        {
            if (o->is( Ramses::instance()->currentUser() ))
            {
                QMessageBox::information(this,
                                         "Can't remove this user",
                                         "Sorry, you can't remove yourself!\nAsk for someone else to remove you, that's safer.");
                return;
            }
        }
        if (m_dontRemove.contains(o->shortName()))
        {
            QMessageBox::information(this,
                                     "Can't remove this",
                                     "Sorry, " + o->shortName() + " | " + o->name() + " must not be removed.");
            return;
        }
        if (o->shortName() == "Duduf" && o->comment() != "")
        {
            QMessageBox::StandardButton result = QMessageBox::question(this,
                                     "Please read this before removing Duduf!",
                                     o->comment() + "\n\nDo you want to go to the website to make a donation?");
            if (result == QMessageBox::Yes)
            {
                QDesktopServices::openUrl(QUrl(URL_DONATION));
                return;
            }
        }
    }

    QList<RO> objs = m_objectList->removeIndices(selection);

    if (m_editMode == RemoveObjects)
    {
        for (int i = objs.count() -1 ; i >= 0; i--)
        {
            objs.at(i)->remove();
        }
    }
}

template<>
void ObjectListEditWidget<RamUser *, int>::removeSelectedObjects()
{
    QModelIndexList selection = ui_listWidget->selectionModel()->selectedRows();
    if (selection.count() == 0) return;

    if (m_editMode == RemoveObjects)
    {
        QMessageBox::StandardButton confirm = QMessageBox::question( this,
            "Confirm deletion",
            "Are you sure you want to permanently remove the selected users?" );

        if ( confirm != QMessageBox::Yes) return;
    }

    // Check if we can remove these objects
    for (int i = 0; i < selection.count(); i++)
    {
        QModelIndex index = selection.at(i);
        quintptr iptr = index.data(Qt::UserRole).toULongLong();
        if(iptr == 0) continue;
        RamUser *o = reinterpret_cast<RamUser *>( iptr );
        // Don't remove yourself if you're a user
        if (o->is( Ramses::instance()->currentUser() ))
        {
            QMessageBox::information(this,
                                     "Can't remove this user",
                                     "Sorry, you can't remove yourself!\nAsk for someone else to remove you, that's safer.");
            return;
        }
        if (m_dontRemove.contains(o->shortName()))
        {
            QMessageBox::information(this,
                                     "Can't remove this",
                                     "Sorry, " + o->shortName() + " | " + o->name() + " must not be removed.");
            return;
        }
        if (o->shortName() == "Duduf" && o->comment() != "")
        {
            QMessageBox::StandardButton result = QMessageBox::question(this,
                                     "Please read this before removing Duduf!",
                                     o->comment() + "\n\nDo you want to go to the website to make a donation?");
            if (result == QMessageBox::Yes)
            {
                QDesktopServices::openUrl(QUrl(URL_DONATION));
                return;
            }
        }
    }

    QList<RamUser *> objs = m_objectList->removeIndices(selection);

    if (m_editMode == RemoveObjects)
    {
        for (int i = objs.count() -1 ; i >= 0; i--)
        {
            objs.at(i)->remove();
        }
    }
}

template<typename RO, typename ROF>
void ObjectListEditWidget<RO,ROF>::edit(RO obj)
{
    obj->edit();
}

template<typename RO, typename ROF>
void ObjectListEditWidget<RO,ROF>::assign(RO obj)
{
    m_objectList->append(obj);
}

template<typename RO, typename ROF>
void ObjectListEditWidget<RO,ROF>::objectAssigned(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent)

    if (!m_useAssignList) return;

    RamObjectFilterModel<ROF> *filteredList = ui_listWidget->filteredList();

    for (int i = first ; i <= last; i++)
    {
        ROF assignedObj = filteredList->at(i);
        ui_assignMenu->setObjectVisible(assignedObj, false);
    }
}

template<typename RO, typename ROF>
void ObjectListEditWidget<RO,ROF>::objectUnassigned(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent)

    if (!m_useAssignList) return;

    RamObjectFilterModel<ROF> *filteredList = ui_listWidget->filteredList();

    for (int i = first ; i <= last; i++)
    {
        RO assignedObj = filteredList->at(i);
        if (!assignedObj) continue;
        ui_assignMenu->setObjectVisible(assignedObj, true);
    }
}

template<typename RO, typename ROF>
void ObjectListEditWidget<RO,ROF>::setSearchFocus()
{
    ui_listWidget->releaseKeyboard();
    ui_searchEdit->setFocus();
}

template<typename RO, typename ROF>
void ObjectListEditWidget<RO,ROF>::setupUi(bool editableObjects, RamUser::UserRole editRole)
{
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->setSpacing(3);
    mainLayout->setContentsMargins(0,0,0,0);

    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    buttonsLayout->setSpacing(3);
    buttonsLayout->setContentsMargins(0,3,0,0);

    ui_title = new QLabel(this);
    ui_title->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    buttonsLayout->addWidget(ui_title);

    ui_filterBox = new RamObjectListComboBox<RamObject *>(true, this);
    ui_filterBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    buttonsLayout->addWidget(ui_filterBox);

    ui_removeButton = new QToolButton(this);
    ui_removeButton->setIcon(QIcon(":/icons/remove"));
    ui_removeButton->setIconSize(QSize(12,12));
    buttonsLayout->addWidget(ui_removeButton);

    ui_addButton = new QToolButton(this);
    ui_addButton->setIcon(QIcon(":/icons/add"));
    ui_addButton->setIconSize(QSize(12,12));
    buttonsLayout->addWidget(ui_addButton);

    mainLayout->addLayout(buttonsLayout);

    ui_searchEdit = new DuQFSearchEdit(this);
    mainLayout->addWidget(ui_searchEdit);

    ui_listWidget = new RamObjectListView<RO>(m_objectList, editableObjects, editRole, RamObjectListView<RO>::List, this);
    mainLayout->addWidget(ui_listWidget);

    mainLayout->setStretch(0, 0);
    mainLayout->setStretch(1, 0);
    mainLayout->setStretch(2, 0);
    mainLayout->setStretch(3, 100);

    this->setLayout(mainLayout);

    setEditable(true);
    // Hide filters until at least one is added
    setFilterList(nullptr);

    ui_assignMenu = new RamObjectListMenu<RO>(false, this);
    ui_assignMenu->addCreateButton();
}

template<typename RO, typename ROF>
void ObjectListEditWidget<RO,ROF>::connectEvents()
{
    // add & remove buttons
    connect(ui_addButton, &QToolButton::clicked, this, &ObjectListEditWidget::add);
    connect(ui_assignMenu, &RamObjectListMenu<RO>::create, this, &ObjectListEditWidget::add);
    connect(ui_removeButton, SIGNAL(clicked()), this, SLOT(removeSelectedObjects()));
    connect(ui_assignMenu,SIGNAL(assign(RamObject*)),this,SLOT(assign(RamObject*)));
    // search
    connect(ui_searchEdit, SIGNAL(changing(QString)), ui_listWidget, SLOT(search(QString)));
    connect(ui_searchEdit, SIGNAL(changed(QString)), ui_listWidget, SLOT(search(QString)));
    // filters
    connect(ui_filterBox,SIGNAL(currentObjectChanged(RamObject*)), this, SLOT(setFilter(RamObject*)));
    // edit objects
    connect(ui_listWidget, SIGNAL(editObject(RamObject*)), this, SLOT(edit(RamObject*)));
    // Relay list signals
    connect(ui_listWidget, &RamObjectListView<RO>::objectSelected, this, &ObjectListEditWidget::objectSelected);

    // Shortcuts
    QShortcut *s = new QShortcut(QKeySequence(QKeySequence::Delete), ui_listWidget, nullptr, nullptr, Qt::WidgetWithChildrenShortcut );
    connect(s, SIGNAL(activated()), this, SLOT(removeSelectedObjects()));
    s = new QShortcut(QKeySequence(QKeySequence::Find), this, nullptr, nullptr, Qt::WidgetWithChildrenShortcut );
    connect(s, SIGNAL(activated()), this, SLOT(setSearchFocus()));
}
