#include "objectlisteditwidget.h"

ObjectListEditWidget::ObjectListEditWidget(RamObjectList *objectList, bool editableObjects, QWidget *parent) : QWidget(parent)
{
    m_objectList = objectList;
    setupUi(objectList, editableObjects);

    setSortable(false);
    setEditable(true);

    // Hide filters until at least one is added
    m_filterLabel->hide();
    m_filterBox->hide();

    connectEvents();
}

RamObjectList *ObjectListEditWidget::list() const
{
    return m_objectList;
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

void ObjectListEditWidget::scrollToBottom()
{
    QScrollBar *vbar = m_list->verticalScrollBar();
    vbar->setSliderPosition( vbar->maximum() );
}

void ObjectListEditWidget::setupUi(RamObjectList *objectList, bool editableObjects)
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
    m_filterBox->addItem("All", "");
    filterLayout->addWidget(m_filterBox);

    mainLayout->addLayout(filterLayout);

    m_list = new RamObjectListWidget(objectList, editableObjects, this);
    mainLayout->addWidget(m_list);

    mainLayout->setStretch(0, 0);
    mainLayout->setStretch(1, 0);
    mainLayout->setStretch(2, 100);

    this->setLayout(mainLayout);
}

void ObjectListEditWidget::connectEvents()
{
    connect(m_addButton, &QToolButton::clicked, this, &ObjectListEditWidget::add);
    connect(m_removeButton, &QToolButton::clicked, m_list, &RamObjectListWidget::removeSelectedObjects);
    // Relay list signals
    connect(m_list, &RamObjectListWidget::objectSelected, this, &ObjectListEditWidget::objectSelected);
    connect(m_list, &RamObjectListWidget::orderChanged, this, &ObjectListEditWidget::orderChanged);
}
