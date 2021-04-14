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

void SimpleObjectList::setSortable(bool sortable)
{
    m_upButton->setVisible(sortable);
    m_downButton->setVisible(sortable);
    m_list->setDragable(sortable);
}

void SimpleObjectList::addObject(RamObject *obj, bool edit)
{
    // Check type to create widget
    RamObject::ObjectType type = obj->objectType();
    RamObjectWidget *ow;
    switch (type)
    {
    case RamObject::Asset:
    {
        RamAsset *a = dynamic_cast<RamAsset*>(obj);
        if (a) ow = new RamAssetWidget(a, this);
        else ow = new RamObjectWidget(obj,this);
        break;
    }
    case RamObject::Shot:
    {
        RamShot *s = dynamic_cast<RamShot*>(obj);
        if (s) ow = new RamShotWidget(s, this);
        else ow = new RamObjectWidget(obj,this);
        break;
    }
    default:
        ow = new RamObjectWidget(obj,this);
        break;
    }

    if (!m_editableObjects) ow->disableEdit();

    QListWidgetItem *i = new QListWidgetItem();
    m_list->addItem(i);
    m_list->setItemWidget(i, ow);

    if (m_editableObjects && edit) ow->edit();

    connect(obj, &RamObject::removed, this, &SimpleObjectList::removeObject);
}

void SimpleObjectList::removeObject(RamObject *obj)
{
    for( int row = 0; row < m_list->count(); row++)
    {
        // Get the object from the widget
        RamObjectWidget *ow = (RamObjectWidget*)m_list->itemWidget( m_list->item(row) );
        if (ow->ramObject()->uuid() == obj->uuid())
        {
            delete m_list->takeItem(row);
        }
    }
}

void SimpleObjectList::removeSelectedObjects()
{
    if (m_list->selectedItems().count() == 0 ) return;
    QMessageBox::StandardButton confirm = QMessageBox::question(this,
                                                                "Confirm deletion",
                                                                "Are you sure you want to premanently remove the selected items?" );
    if (confirm != QMessageBox::Yes) return;

    for(int row =  m_list->count() -1 ; row >= 0; row--)
    {
        QListWidgetItem *i = m_list->item(row);
        if (!i->isSelected()) continue;
        RamObjectWidget *ow = (RamObjectWidget*)m_list->itemWidget( i );
        if (ow) emit objectRemoved(ow->ramObject());
        delete m_list->takeItem(row);
    }
}

void SimpleObjectList::clear()
{
    m_list->clear();
}

void SimpleObjectList::currentItemChanged(QListWidgetItem *previous, QListWidgetItem *current)
{
    Q_UNUSED(previous);
    if (!current) return;
    RamObjectWidget *ow = (RamObjectWidget*)m_list->itemWidget( current );
    if (!ow) return;
    emit objectSelected(ow->ramObject());
}

void SimpleObjectList::selectionChanged()
{
    for(int row = 0; row < m_list->count(); row++)
    {
        QListWidgetItem *i = m_list->item(row);
        RamObjectWidget *ow = (RamObjectWidget*)m_list->itemWidget( i );
        if (ow) ow->setSelected( i->isSelected() );
    }
}

void SimpleObjectList::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->setSpacing(3);
    mainLayout->setContentsMargins(0,0,0,0);

    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    buttonsLayout->setSpacing(3);
    buttonsLayout->setContentsMargins(0,0,0,0);

    buttonsLayout->addStretch();

    m_downButton = new QToolButton(this);
    m_downButton->setIcon(QIcon(":/icons/move-down"));
    m_downButton->setIconSize(QSize(12,12));
    buttonsLayout->addWidget(m_downButton);

    m_upButton = new QToolButton(this);
    m_upButton->setIcon(QIcon(":/icons/move-up"));
    m_upButton->setIconSize(QSize(12,12));
    buttonsLayout->addWidget(m_upButton);

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

    m_list = new DuQFListWidget(this);
    mainLayout->addWidget(m_list);

    mainLayout->setStretch(0, 0);
    mainLayout->setStretch(1, 0);
    mainLayout->setStretch(2, 100);

    this->setLayout(mainLayout);
}

void SimpleObjectList::connectEvents()
{
    connect(m_addButton, &QToolButton::clicked, this, &SimpleObjectList::add);
    connect(m_removeButton, &QToolButton::clicked, this, &SimpleObjectList::removeSelectedObjects);
    connect(m_list, &QListWidget::currentItemChanged, this, &SimpleObjectList::currentItemChanged);
    connect(m_list, &QListWidget::itemSelectionChanged, this, &SimpleObjectList::selectionChanged);
}
