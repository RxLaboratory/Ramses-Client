#include "objectlisteditwidget.h"

ObjectListEditWidget::ObjectListEditWidget(bool editableObjects, RamUser::UserRole editRole, QWidget *parent) :
    QWidget(parent)
{
    m_objectList = nullptr;
    setupUi(editableObjects, editRole);
    connectEvents();
}

ObjectListEditWidget::ObjectListEditWidget(RamObjectList *objectList, bool editableObjects, RamUser::UserRole editRole, QWidget *parent) :
    QWidget(parent)
{
    setupUi(editableObjects, editRole);
    connectEvents();
    setList(objectList);
}

void ObjectListEditWidget::setList(RamObjectList *objectList)
{
    while(!m_listConnections.isEmpty()) disconnect(m_listConnections.takeLast());

    setFilterList(nullptr);
    m_objectList = objectList;
    ui_listWidget->setList(m_objectList);

    if (!objectList) return;

    // assignment
    m_listConnections << connect(ui_listWidget->filteredList(), SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(objectAssigned(QModelIndex,int,int)));
    m_listConnections << connect(ui_listWidget->filteredList(), SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)), this, SLOT(objectUnassigned(QModelIndex,int,int)));

    objectAssigned(QModelIndex(), 0, m_objectList->count() - 1);
}

void ObjectListEditWidget::setFilterList(RamObjectList *filterList)
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

void ObjectListEditWidget::setAssignList(RamObjectList *assignList)
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

void ObjectListEditWidget::setDontRemoveShortNameList(QStringList dontRemove)
{
    m_dontRemove = dontRemove;
}

void ObjectListEditWidget::clear()
{
    ui_listWidget->setList(nullptr);
}

void ObjectListEditWidget::setEditMode(EditMode editMode)
{
    m_editMode = editMode;
}

void ObjectListEditWidget::setEditable(bool editable)
{
    ui_removeButton->setVisible(editable);
    ui_addButton->setVisible(editable);
}

void ObjectListEditWidget::setSearchable(bool searchable)
{
    ui_searchEdit->setVisible(searchable);
}

void ObjectListEditWidget::setSortable(bool sortable)
{
    ui_listWidget->setSortable(sortable);
}

void ObjectListEditWidget::setTitle(QString title)
{
    ui_title->setVisible(title != "");
    ui_title->setText(title);
}

void ObjectListEditWidget::select(RamObject *o)
{
    ui_listWidget->select(o);
}

void ObjectListEditWidget::setFilter(RamObject *o)
{
    QSignalBlocker b(ui_filterBox);
    ui_filterBox->setObject(o);
    ui_assignMenu->filter(o);
    ui_listWidget->filter(o);
}

QToolButton *ObjectListEditWidget::addButton() const
{
    return ui_addButton;
}

QString ObjectListEditWidget::currentFilterUuid() const
{
    return ui_filterBox->currentUuid();
}

RamObject *ObjectListEditWidget::currentFilter() const
{
    return ui_filterBox->currentObject();
}

void ObjectListEditWidget::removeSelectedObjects()
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
        RamObject *o = reinterpret_cast<RamObject*>( iptr );
        if (m_dontRemove.contains(o->shortName()))
        {
            QMessageBox::information(this,
                                     "Can't remove this",
                                     "Sorry, " + o->shortName() + " | " + o->name() + " must not be removed.");
            return;
        }
    }

    QList<RamObject*> objs = m_objectList->removeIndices(selection);

    if (m_editMode == RemoveObjects)
    {
        for (int i = objs.count() -1 ; i >= 0; i--)
        {
            objs.at(i)->remove();
        }
    }
}

void ObjectListEditWidget::edit(RamObject *obj)
{
    obj->edit();
}

void ObjectListEditWidget::assign(RamObject *obj)
{
    m_objectList->append(obj);
}

void ObjectListEditWidget::objectAssigned(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent)

    if (!m_useAssignList) return;

    RamObjectFilterModel *filteredList = ui_listWidget->filteredList();

    for (int i = first ; i <= last; i++)
    {
        RamObject *assignedObj = reinterpret_cast<RamObject*>( filteredList->data( filteredList->index(i,0), Qt::UserRole ).toULongLong() );
        ui_assignMenu->setObjectVisible(assignedObj, false);
    }
}

void ObjectListEditWidget::objectUnassigned(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent)

    if (!m_useAssignList) return;

    RamObjectFilterModel *filteredList = ui_listWidget->filteredList();

    for (int i = first ; i <= last; i++)
    {
        RamObject *assignedObj = reinterpret_cast<RamObject*>( filteredList->data( filteredList->index(i,0), Qt::UserRole ).toULongLong() );
        ui_assignMenu->setObjectVisible(assignedObj, true);
    }
}

void ObjectListEditWidget::setSearchFocus()
{
    ui_listWidget->releaseKeyboard();
    ui_searchEdit->setFocus();
}

void ObjectListEditWidget::setupUi(bool editableObjects, RamUser::UserRole editRole)
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

    ui_filterBox = new RamObjectListComboBox(true, this);
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

    ui_listWidget = new RamObjectListWidget(m_objectList, editableObjects, editRole, RamObjectListWidget::List, this);
    mainLayout->addWidget(ui_listWidget);

    mainLayout->setStretch(0, 0);
    mainLayout->setStretch(1, 0);
    mainLayout->setStretch(2, 0);
    mainLayout->setStretch(3, 100);

    this->setLayout(mainLayout);

    setEditable(true);
    // Hide filters until at least one is added
    setFilterList(nullptr);

    ui_assignMenu = new RamObjectListMenu(false, this);
    ui_assignMenu->addCreateButton();
}

void ObjectListEditWidget::connectEvents()
{
    // add & remove buttons
    connect(ui_addButton, &QToolButton::clicked, this, &ObjectListEditWidget::add);
    connect(ui_assignMenu, &RamObjectListMenu::create, this, &ObjectListEditWidget::add);
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
    connect(ui_listWidget, &RamObjectListWidget::objectSelected, this, &ObjectListEditWidget::objectSelected);

    // Shortcuts
    QShortcut *s = new QShortcut(QKeySequence(QKeySequence::Delete), ui_listWidget, nullptr, nullptr, Qt::WidgetWithChildrenShortcut );
    connect(s, SIGNAL(activated()), this, SLOT(removeSelectedObjects()));
    s = new QShortcut(QKeySequence(QKeySequence::Find), this, nullptr, nullptr, Qt::WidgetWithChildrenShortcut );
    connect(s, SIGNAL(activated()), this, SLOT(setSearchFocus()));
}
