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
    m_objectList = objectList;
    m_list->setList(m_objectList);
}

void ObjectListEditWidget::setList(RamObjectUberList *objectList)
{
    while (!m_uberListConnections.isEmpty()) disconnect( m_uberListConnections.takeLast() );

    m_objectList = nullptr;
    m_objectUberList = objectList;
    m_list->setList(objectList);

    clearFilters();
    m_filterBox->addItem("All", "");
    for (int i = 0; i < objectList->count(); i++) newFilter( objectList->at(i) );

    if (objectList->count() > 1)
    {
        m_filterBox->show();
        //m_filterLabel->show();
    }

    m_uberListConnections << connect( objectList, &RamObjectList::objectAdded, this, &ObjectListEditWidget::newFilter);
    m_uberListConnections << connect( objectList, &RamObjectList::objectRemoved, this, &ObjectListEditWidget::filterRemoved);
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

QString ObjectListEditWidget::currentFilter() const
{
    return m_filterBox->currentData().toString();
}

void ObjectListEditWidget::clearFilters()
{
    m_filterBox->clear();
    m_filterBox->hide();
    m_filterLabel->hide();
}

void ObjectListEditWidget::scrollToBottom()
{
    QScrollBar *vbar = m_list->verticalScrollBar();
    vbar->setSliderPosition( vbar->maximum() );
}

void ObjectListEditWidget::newFilter(RamObject *filter)
{
    m_filterBox->addItem( filter->name(), filter->uuid() );

    QList<QMetaObject::Connection> c;
    c << connect(filter, &RamObject::removed, this, &ObjectListEditWidget::filterRemoved);
    c << connect(filter, &RamObject::changed, this, &ObjectListEditWidget::filterChanged);
    m_filterConnections[filter->uuid()] = c;
}

void ObjectListEditWidget::filterRemoved(RamObject *filter)
{
    if (m_filterConnections.contains(filter->uuid()))
    {
        QList<QMetaObject::Connection> c = m_filterConnections.value(filter->uuid());
        while (!c.isEmpty()) disconnect( c.takeLast());
    }

    for (int i = m_filterBox->count() - 1; i >= 0 ; i--)
    {
        if (m_filterBox->itemData(i).toString() == filter->uuid())
        {
            m_filterBox->removeItem(i);
        }
    }
}

void ObjectListEditWidget::filterChanged(RamObject *filter)
{
    for (int i = m_filterBox->count() - 1; i >= 0 ; i--)
    {
        if (m_filterBox->itemData(i).toString() == filter->uuid())
        {
            m_filterBox->setItemText(i, filter->name());
        }
    }
}

void ObjectListEditWidget::currentFilterChanged(int i)
{
    if (i <= 0) m_list->filter("");
    else m_list->filter( m_filterBox->currentData().toString() );
}

void ObjectListEditWidget::setupUi(bool editableObjects)
{
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->setSpacing(3);
    mainLayout->setContentsMargins(0,0,0,0);

    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    buttonsLayout->setSpacing(3);
    buttonsLayout->setContentsMargins(0,0,0,0);

    m_title = new QLabel(this);
    m_title->setVisible(false);
    buttonsLayout->addWidget(m_title);

    buttonsLayout->addStretch();

    m_removeButton = new QToolButton(this);
    m_removeButton->setIcon(QIcon(":/icons/remove"));
    m_removeButton->setIconSize(QSize(12,12));
    buttonsLayout->addWidget(m_removeButton);

    m_addButton = new QToolButton(this);
    m_addButton->setIcon(QIcon(":/icons/add"));
    m_addButton->setIconSize(QSize(12,12));
    buttonsLayout->addWidget(m_addButton);

    mainLayout->addLayout(buttonsLayout);

    QHBoxLayout *filterLayout = new QHBoxLayout();
    filterLayout->setSpacing(3);
    filterLayout->setContentsMargins(0,0,0,0);

    m_filterLabel = new QLabel(this);
    m_filterLabel->setMaximumSize(QSize(12,12));
    m_filterLabel->setScaledContents(true);
    m_filterLabel->setPixmap(QPixmap(":/icons/filter"));
    filterLayout->addWidget(m_filterLabel);

    m_filterBox = new QComboBox(this);
    filterLayout->addWidget(m_filterBox);

    mainLayout->addLayout(filterLayout);

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
    clearFilters();
}

void ObjectListEditWidget::connectEvents()
{
    connect(m_addButton, &QToolButton::clicked, this, &ObjectListEditWidget::add);
    connect(m_removeButton, &QToolButton::clicked, m_list, &RamObjectListWidget::removeSelectedObjects);
    connect(m_searchEdit, &DuQFSearchEdit::changing, m_list, &RamObjectListWidget::search);
    connect(m_searchEdit, &DuQFSearchEdit::changed, m_list, &RamObjectListWidget::search);
    connect(m_filterBox,SIGNAL(currentIndexChanged(int)), this, SLOT(currentFilterChanged(int)));
    // Relay list signals
    connect(m_list, &RamObjectListWidget::objectSelected, this, &ObjectListEditWidget::objectSelected);
    connect(m_list, &RamObjectListWidget::orderChanged, this, &ObjectListEditWidget::orderChanged);
}
