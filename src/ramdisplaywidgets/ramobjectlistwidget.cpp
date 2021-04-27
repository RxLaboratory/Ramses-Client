#include "ramobjectlistwidget.h"

RamObjectListWidget::RamObjectListWidget(RamObjectList *list, QWidget *parent):
    QTableWidget(parent)
{
    m_list = list;
    setupUi();
    connectEvents();
}

RamObjectListWidget::RamObjectListWidget(RamObjectList *list, bool editableObjects, QWidget *parent):
    QTableWidget(parent)
{
    m_list = list;
    m_editableObjects = editableObjects;
    setupUi();
    connectEvents();
}

void RamObjectListWidget::setSortable(bool sortable)
{
    this->verticalHeader()->setVisible(sortable);
}

void RamObjectListWidget::setSelectable(bool selectable)
{
    if(selectable) this->setSelectionMode(ExtendedSelection);
    else this->setSelectionMode(NoSelection);
}

RamObjectList *RamObjectListWidget::objects() const
{
    return m_list;
}

void RamObjectListWidget::select(RamObject *obj)
{
    for (int row =0; row < this->rowCount(); row++)
    {
        // Get the object from the widget
        RamObjectWidget *ow = (RamObjectWidget*)this->cellWidget(row, 0 );
        if (ow->ramObject()->is(obj))
        {
            this->item(row, 0)->setSelected(true);
        }
    }
}

void RamObjectListWidget::removeSelectedObjects()
{
    if (this->selectedItems().count() == 0 ) return;
    QMessageBox::StandardButton confirm = QMessageBox::question(this,
                                                                "Confirm deletion",
                                                                "Are you sure you want to premanently remove the selected items?" );
    if (confirm != QMessageBox::Yes) return;

    for(int row =  this->rowCount() -1 ; row >= 0; row--)
    {
        QTableWidgetItem *i = this->item(row, 0);
        if (!i->isSelected()) continue;
        RamObjectWidget *ow = (RamObjectWidget*) this->cellWidget(row, 0);
        if (ow)
        {
            ow->ramObject()->remove();
        }
    }
}

void RamObjectListWidget::unassignSelectedObjects()
{
    for(int row =  this->rowCount() -1 ; row >= 0; row--)
    {
        QTableWidgetItem *i = this->item(row, 0);
        if (!i->isSelected()) continue;
        RamObjectWidget *ow = (RamObjectWidget*) this->cellWidget(row, 0);
        if (ow)
        {
            m_list->removeAll(ow->ramObject());
        }
    }
}

void RamObjectListWidget::resizeEvent(QResizeEvent *event)
{
    this->setColumnWidth( 0, event->size().width() );
    this->setRowHeight(0, 10);
    this->resizeRowsToContents();
}

void RamObjectListWidget::itemSelected(QTableWidgetItem *current, QTableWidgetItem *previous)
{
    Q_UNUSED(previous);
    if (!current) return;
    RamObjectWidget *ow = (RamObjectWidget*)this->cellWidget( current->row(), 0 );
    if (!ow) return;
    ow->setSelected(current->isSelected());
    emit objectSelected(ow->ramObject());
}

void RamObjectListWidget::changeSelection()
{
    for(int row = 0; row < this->rowCount(); row++)
    {
        QTableWidgetItem *i = this->item(row, 0);
        RamObjectWidget *ow = (RamObjectWidget*) this->cellWidget( row, 0 );
        if (ow) ow->setSelected( i->isSelected() );
    }
}

void RamObjectListWidget::updateOrder()
{
    for(int row = 0; row < this->rowCount(); row++)
    {
        RamObjectWidget *ow = (RamObjectWidget*) this->cellWidget( row, 0 );
        if (ow)
        {
            RamObject *o = ow->ramObject();
            if (o)
            {
                o->setOrder(this->verticalHeader()->visualIndex(row));
                o->update();
            }
        }
    }
    emit orderChanged();
}

void RamObjectListWidget::objectChanged(RamObject *obj)
{
    for( int row = 0; row < this->rowCount(); row++)
    {
        // Get the object from the widget
        RamObjectWidget *ow = (RamObjectWidget*)this->cellWidget(row, 0 );
        if (ow->ramObject()->is(obj))
        {
            this->item(row, 0)->setText("   " + obj->name());
        }
    }
}

void RamObjectListWidget::objectUnassigned(RamObject *obj)
{
    if (m_objectConnections.contains(obj->uuid()))
    {
        disconnect( m_objectConnections.take(obj->uuid()) );
    }

    for( int row = 0; row < this->rowCount(); row++)
    {
        // Get the object from the widget
        RamObjectWidget *ow = (RamObjectWidget*)this->cellWidget(row, 0 );
        if (ow->ramObject()->is(obj))
        {
            delete ow;
            this->removeRow(row);
        }
    }
}

void RamObjectListWidget::objectAssigned(RamObject *obj)
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
    case RamObject::Status:
    {
        RamStatus *s = dynamic_cast<RamStatus*>(obj);
        if (s) ow = new RamStatusWidget(s, this);
        else ow = new RamObjectWidget(obj, this);
        break;
    }
    case RamObject::User:
    {
        RamUser *u = dynamic_cast<RamUser*>(obj);
        if (u) ow = new RamUserWidget(u, this);
        else ow = new RamObjectWidget(obj, this);
        break;
    }
    case RamObject::State:
    {
        RamState *s = dynamic_cast<RamState*>(obj);
        if (s) ow = new RamStateWidget(s, this);
        else ow = new RamObjectWidget(obj, this);
        break;
    }
    default:
        ow = new RamObjectWidget(obj,this);
        break;
    }

    ow->setEditable(m_editableObjects);

    int row = this->rowCount();
    this->setRowCount( row + 1 );
    this->setItem(row, 0, new QTableWidgetItem("   " + obj->name()));
    this->setCellWidget(row, 0, ow);
    this->resizeRowToContents(row);

    if (m_editableObjects && this->isVisible() && obj->shortName() == "NEW") ow->edit();

    m_objectConnections[obj->uuid()] = connect(obj, &RamObject::changed, this, &RamObjectListWidget::objectChanged);
}

void RamObjectListWidget::setupUi()
{
    setSortable(true);
    this->setSelectionMode(NoSelection);
    this->setFrameShape(QFrame::NoFrame);
    this->setDragDropMode(NoDragDrop);
    this->setDragEnabled(false);
    this->setDefaultDropAction(Qt::IgnoreAction);
    this->verticalHeader()->setSectionsMovable(true);

    this->setColumnCount(1);
    this->setRowCount(0);

    this->verticalHeader()->setVisible(false);
    this->horizontalHeader()->setVisible(false);

    this->setColumnWidth( 0, this->size().width() );

    this->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

    int p = DuUI::getSize("padding", "large");
    QString padding = QString::number(p) + "px";
    QString style = "QTableWidget { gridline-color: rgba(0,0,0,0); padding-top:" + padding +  "; padding-bottom:" + padding +  "; selection-background-color: rgba(0,0,0,0); } ";
    style += "QTableWidget::item { padding-left: " + padding + "; padding-right: " + padding + "; } ";
    style += "QTableWidget::item:hover { background-color: none; } ";

    this->setStyleSheet(style);
}

void RamObjectListWidget::connectEvents()
{
    connect(m_list, &RamObjectList::objectRemoved, this, &RamObjectListWidget::objectUnassigned);
    connect(m_list, &RamObjectList::objectAdded, this, &RamObjectListWidget::objectAssigned);

    connect(this->verticalHeader(), &QHeaderView::sectionMoved, this, &RamObjectListWidget::updateOrder);
    connect(this, &QTableWidget::currentItemChanged, this, &RamObjectListWidget::itemSelected);
    connect(this, &QTableWidget::itemSelectionChanged, this, &RamObjectListWidget::changeSelection);
}
