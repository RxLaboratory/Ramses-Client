#include "objectlisteditwidget.h"

ObjectListEditWidget::ObjectListEditWidget(bool editableObjects, QWidget *parent) :
    QWidget(parent)
{
    m_objectList = nullptr;
    m_objectUberList = nullptr;
    setupUi(editableObjects);
    connectEvents();
}

ObjectListEditWidget::ObjectListEditWidget(RamObjectList *objectList, bool editableObjects, QWidget *parent) :
    QWidget(parent)
{
    setupUi(editableObjects);
    connectEvents();
    setList(objectList);
}

ObjectListEditWidget::ObjectListEditWidget(RamObjectUberList *objectList, bool editableObjects, QWidget *parent) :
    QWidget(parent)
{
    setupUi(editableObjects);
    connectEvents();
    setList(objectList);
}

void ObjectListEditWidget::setContainingType(RamObject::ObjectType type)
{
    m_list->setContainingType(type);
}

void ObjectListEditWidget::setEditMode(const RamObjectListWidget::EditMode &editMode)
{
    m_list->setEditMode(editMode);
}

RamObjectList *ObjectListEditWidget::list() const
{
    if (m_objectUberList) return m_objectUberList;
    return m_objectList;
}

void ObjectListEditWidget::clear()
{
    m_objectList = nullptr;
    m_objectUberList = nullptr;
    m_list->clear();
}

void ObjectListEditWidget::setList(RamObjectList *objectList)
{
    setFilterList(nullptr);
    m_objectList = objectList;
    m_list->setList(m_objectList);
}

void ObjectListEditWidget::setList(RamObjectUberList *objectList, bool useFilters)
{
    while (!m_uberListConnections.isEmpty()) disconnect( m_uberListConnections.takeLast() );

    m_objectList = nullptr;
    m_objectUberList = objectList;
    m_list->setList(objectList);

    if (useFilters)
    {
        setFilterList( objectList );
    }
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
    }//*/
}

void ObjectListEditWidget::setEditable(bool editable)
{
    m_removeButton->setVisible(editable);
    m_addButton->setVisible(editable);
    setSelectable(editable);
}

void ObjectListEditWidget::setSortable(bool sortable)
{
    m_list->setSortable(sortable);
}

void ObjectListEditWidget::setSelectable(bool selectable)
{
    m_list->setSelectable(selectable);
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

QToolButton *ObjectListEditWidget::addButton() const
{
    return m_addButton;
}

void ObjectListEditWidget::select(RamObject *obj)
{
    m_list->select(obj);
}

QString ObjectListEditWidget::currentFilterUuid() const
{
    return m_filterBox->currentUuid();
}

RamObject *ObjectListEditWidget::currentFilter() const
{
    return m_filterBox->currentObject();
}

void ObjectListEditWidget::scrollToBottom()
{
    QScrollBar *vbar = m_list->verticalScrollBar();
    vbar->setSliderPosition( vbar->maximum() );
}

void ObjectListEditWidget::filterChanged(QString filter)
{
    m_list->filter( filter );
    emit currentFilterChanged( filter );
}

void ObjectListEditWidget::setupUi(bool editableObjects)
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

    m_list = new RamObjectListWidget(m_objectList, editableObjects, this);
    mainLayout->addWidget(m_list);

    mainLayout->setStretch(0, 0);
    mainLayout->setStretch(1, 0);
    mainLayout->setStretch(2, 0);
    mainLayout->setStretch(3, 100);

    this->setLayout(mainLayout);

    setSortable(false);
    setEditable(true);
    // Hide filters until at least one is added
    setFilterList(nullptr);
}

void ObjectListEditWidget::connectEvents()
{
    connect(m_addButton, &QToolButton::clicked, this, &ObjectListEditWidget::add);
    connect(m_removeButton, &QToolButton::clicked, m_list, &RamObjectListWidget::removeSelectedObjects);
    connect(m_searchEdit, &DuQFSearchEdit::changing, m_list, &RamObjectListWidget::search);
    connect(m_searchEdit, &DuQFSearchEdit::changed, m_list, &RamObjectListWidget::search);
    connect(m_filterBox,SIGNAL(currentObjectChanged(QString)), this, SLOT(filterChanged(QString)));
    // Relay list signals
    connect(m_list, &RamObjectListWidget::objectSelected, this, &ObjectListEditWidget::objectSelected);
    connect(m_list, &RamObjectListWidget::orderChanged, this, &ObjectListEditWidget::orderChanged);
}
