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
    m_listWidget->setList(m_objectList);

    if (!objectList) return;

    // assignment
    m_listConnections << connect(objectList, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(objectAssigned(QModelIndex,int,int)));
    m_listConnections << connect(objectList, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)), this, SLOT(objectUnassigned(QModelIndex,int,int)));

    objectAssigned(QModelIndex(), 0, m_objectList->count() - 1);
}

void ObjectListEditWidget::setFilterList(RamObjectList *filterList)
{
    m_filterBox->setList(filterList);
    m_filterList = filterList;
    if (filterList)
    {
        m_title->hide();
        m_filterBox->show();
    }
    else
    {
        m_title->show();
        m_filterBox->hide();
    }
}

void ObjectListEditWidget::setAssignList(RamObjectList *assignList)
{
    m_assignMenu = new QMenu(this);
    m_addButton->setPopupMode(QToolButton::InstantPopup);
    m_addButton->setMenu(m_assignMenu);

    m_assignList = assignList;

    // Add Actions
    QAction *addAction = new QAction("Create new");
    m_assignMenu->addAction(addAction);
    connect(addAction, &QAction::triggered, this, &ObjectListEditWidget::add);

    m_assignMenu->addSeparator();

    // add one action per obj
    for (int i = 0; i < assignList->count(); i++)
        newAssignObj( assignList->at(i) );

    // hide already assigned
    if(m_objectList)
    {
        if (m_objectList->count())
            objectAssigned(QModelIndex(), 0, m_objectList->count() - 1);
    }

    m_useAssignList = true;

    connect(assignList, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(newAssignObj(QModelIndex,int,int)));
    connect(assignList, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)), this, SLOT(assignObjRemoved(QModelIndex,int,int)));
}

void ObjectListEditWidget::clear()
{
    m_listWidget->setList(nullptr);
}

void ObjectListEditWidget::setEditMode(EditMode editMode)
{
    m_editMode = editMode;
}

void ObjectListEditWidget::setEditable(bool editable)
{
    m_removeButton->setVisible(editable);
    m_addButton->setVisible(editable);
}

void ObjectListEditWidget::setSearchable(bool searchable)
{
    m_searchEdit->setVisible(searchable);
}

void ObjectListEditWidget::setTitle(QString title)
{
    m_title->setVisible(title != "");
    m_title->setText(title);
}

void ObjectListEditWidget::select(RamObject *o)
{
    m_listWidget->select(o);
}

void ObjectListEditWidget::setFilter(RamObject *o)
{
    m_filterBox->setObject(o);
    m_listWidget->filter(o);
}

QToolButton *ObjectListEditWidget::addButton() const
{
    return m_addButton;
}

QString ObjectListEditWidget::currentFilterUuid() const
{
    return m_filterBox->currentUuid();
}

RamObject *ObjectListEditWidget::currentFilter() const
{
    return m_filterBox->currentObject();
}

void ObjectListEditWidget::removeSelectedObjects()
{
    QModelIndexList selection = m_listWidget->selectionModel()->selectedRows();
    if (selection.count() == 0) return;


    if (m_editMode == RemoveObjects)
    {
        QMessageBox::StandardButton confirm = QMessageBox::question( this,
            "Confirm deletion",
            "Are you sure you want to premanently remove the selected items?" );

        if ( confirm != QMessageBox::Yes) return;
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
    qDebug() << "List Edit Widget EDIT";
    obj->edit();
}

void ObjectListEditWidget::newAssignObj(RamObject *obj)
{
    QAction *objAction = new QAction( obj->name() );
    quintptr iptr = reinterpret_cast<quintptr>( obj );
    objAction->setData(iptr);
    m_assignMenu->addAction(objAction);
    connect(objAction, SIGNAL(triggered()), this, SLOT(assignAction()));
    connect(obj, SIGNAL(changed(RamObject*)), this, SLOT(assignObjChanged(RamObject*)));
}

void ObjectListEditWidget::newAssignObj(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent)

    for (int i = first; i <= last; i++)
    {
        RamObject *o = m_assignList->at(i);
        newAssignObj(o);
    }
}

void ObjectListEditWidget::assignObjRemoved(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent)

    QList<QAction *> actions = m_assignMenu->actions();

    for (int i = first; i <= last; i++)
    {
        RamObject *removedObj = m_assignList->at(i);
        for (int j = actions.count() -1 ; j >= 2 ; j++)
        {
            quintptr iptr = actions.at(j)->data().toULongLong();
            RamObject *obj = reinterpret_cast<RamObject*>( iptr );

            if (removedObj->is(obj)) actions.at(j)->deleteLater();
            break;
        }
    }
}

void ObjectListEditWidget::assignObjChanged(RamObject *changedObj)
{
    QList<QAction *> actions = m_assignMenu->actions();

    for(int i= actions.count() -1; i >= 2; i--)
    {
        quintptr iptr = actions.at(i)->data().toULongLong();
        RamObject *obj = reinterpret_cast<RamObject*>( iptr );

        if (changedObj->is(obj)) actions.at(i)->setText(changedObj->name());
    }
}

void ObjectListEditWidget::assignAction()
{
    QAction *objAction = qobject_cast<QAction*>( sender() );
    quintptr iptr = objAction->data().toULongLong();
    RamObject *obj = reinterpret_cast<RamObject*>( iptr );
    m_objectList->append(obj);
}

void ObjectListEditWidget::objectAssigned(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent)

    if (!m_useAssignList) return;

    QList<QAction *> actions = m_assignMenu->actions();
    for (int i = first ; i <= last; i++)
    {
        RamObject *assignedObj = m_objectList->at(i);
        for(int i= actions.count() -1; i >= 2; i--)
        {
            quintptr iptr = actions.at(i)->data().toULongLong();
            RamObject *obj = reinterpret_cast<RamObject*>( iptr );

            if (assignedObj->is(obj)) actions.at(i)->setVisible(false);
        }
    }
}

void ObjectListEditWidget::objectUnassigned(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent)

    if (!m_useAssignList) return;

    QList<QAction *> actions = m_assignMenu->actions();
    for (int i = first ; i <= last; i++)
    {
        RamObject *unassignedObj = m_objectList->at(i);
        for(int i= actions.count() -1; i >= 2; i--)
        {
            quintptr iptr = actions.at(i)->data().toULongLong();
            RamObject *obj = reinterpret_cast<RamObject*>( iptr );

            if (unassignedObj->is(obj)) actions.at(i)->setVisible(true);
        }
    }
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

    m_title = new QLabel(this);
    m_title->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    buttonsLayout->addWidget(m_title);

    m_filterBox = new RamObjectListComboBox(true, this);
    m_filterBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    buttonsLayout->addWidget(m_filterBox);

    m_removeButton = new QToolButton(this);
    m_removeButton->setIcon(QIcon(":/icons/remove"));
    m_removeButton->setIconSize(QSize(12,12));
    buttonsLayout->addWidget(m_removeButton);

    m_addButton = new QToolButton(this);
    m_addButton->setIcon(QIcon(":/icons/add"));
    m_addButton->setIconSize(QSize(12,12));
    buttonsLayout->addWidget(m_addButton);

    mainLayout->addLayout(buttonsLayout);

    m_searchEdit = new DuQFSearchEdit(this);
    mainLayout->addWidget(m_searchEdit);

    m_listWidget = new RamObjectListWidget(m_objectList, editableObjects, editRole, this);
    mainLayout->addWidget(m_listWidget);

    mainLayout->setStretch(0, 0);
    mainLayout->setStretch(1, 0);
    mainLayout->setStretch(2, 0);
    mainLayout->setStretch(3, 100);

    this->setLayout(mainLayout);

    setEditable(true);
    // Hide filters until at least one is added
    setFilterList(nullptr);
}

void ObjectListEditWidget::connectEvents()
{
    // add & remove buttons
    connect(m_addButton, &QToolButton::clicked, this, &ObjectListEditWidget::add);
    connect(m_removeButton, SIGNAL(clicked()), this, SLOT(removeSelectedObjects()));
    // search
    connect(m_searchEdit, SIGNAL(changing(QString)), m_listWidget, SLOT(search(QString)));
    connect(m_searchEdit, SIGNAL(changed(QString)), m_listWidget, SLOT(search(QString)));
    // filters
    connect(m_filterBox,SIGNAL(currentObjectChanged(RamObject*)), this, SLOT(setFilter(RamObject*)));
    // edit objects
    connect(m_listWidget, SIGNAL(editObject(RamObject*)), this, SLOT(edit(RamObject*)));
    // Relay list signals
    connect(m_listWidget, &RamObjectListWidget::objectSelected, this, &ObjectListEditWidget::objectSelected);
}
