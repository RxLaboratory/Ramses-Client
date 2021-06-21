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
    setFilterList(nullptr);
    m_objectList = objectList;
    m_listWidget->setList(m_objectList);
}

void ObjectListEditWidget::setFilterList(RamObjectList *filterList)
{
    m_filterBox->setList(filterList);
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

void ObjectListEditWidget::filterChanged(QString filter)
{
    //m_list->filter( filter );
    emit currentFilterChanged( filter );
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
    connect(m_filterBox,SIGNAL(currentObjectChanged(QString)), this, SLOT(filterChanged(QString)));
    // edit objects
    connect(m_listWidget, SIGNAL(editObject(RamObject*)), this, SLOT(edit(RamObject*)));
    // Relay list signals
    connect(m_listWidget, &RamObjectListWidget::objectSelected, this, &ObjectListEditWidget::objectSelected);
}
