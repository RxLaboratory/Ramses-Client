#include "simpleobjectlist.h"

SimpleObjectList::SimpleObjectList(bool editableObjects, QWidget *parent) : QWidget(parent)
{
    setupUi();

    setSortable(false);
    m_editableObjects = editableObjects;

    // Hide filters until at least one is added
    m_filterLabel->hide();
    m_filterBox->hide();

    connectEvents();
}

void SimpleObjectList::setEditable(bool editable)
{
    m_removeButton->setVisible(editable);
    m_addButton->setVisible(editable);
}

void SimpleObjectList::setSortable(bool sortable)
{
    m_list->setDragable(sortable);
}

void SimpleObjectList::setTitle(QString title)
{
    m_title->setVisible(title != "");
    m_title->setText(title);
}

void SimpleObjectList::addObject(RamObject *obj, bool edit)
{
    m_list->addObject(obj, edit);
}

void SimpleObjectList::removeObject(RamObject *obj)
{
    m_list->removeObject(obj->uuid());
}

void SimpleObjectList::removeObject(QString uuid)
{
    m_list->removeObject(uuid);
}

QList<RamObject *> SimpleObjectList::ramObjects() const
{
    return m_list->objects();
}

QToolButton *SimpleObjectList::addButton() const
{
    return m_addButton;
}

void SimpleObjectList::clear()
{
    m_list->clear();
    m_list->setRowCount(0);
}

void SimpleObjectList::scrollToBottom()
{
    QScrollBar *vbar = m_list->verticalScrollBar();
    vbar->setSliderPosition( vbar->maximum() );
}

void SimpleObjectList::setupUi()
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

    m_list = new ObjectListWidget(this);
    mainLayout->addWidget(m_list);

    mainLayout->setStretch(0, 0);
    mainLayout->setStretch(1, 0);
    mainLayout->setStretch(2, 100);

    this->setLayout(mainLayout);
}

void SimpleObjectList::connectEvents()
{
    connect(m_addButton, &QToolButton::clicked, this, &SimpleObjectList::add);
    connect(m_removeButton, &QToolButton::clicked, m_list, &ObjectListWidget::removeSelectedObjects);
    // Relay list signals
    connect(m_list, &ObjectListWidget::objectSelected, this, &SimpleObjectList::objectSelected);
    connect(m_list, &ObjectListWidget::objectRemoved, this, &SimpleObjectList::objectRemoved);
    connect(m_list, &ObjectListWidget::orderChanged, this, &SimpleObjectList::orderChanged);
}
