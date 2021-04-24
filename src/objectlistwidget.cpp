#include "objectlistwidget.h"

ObjectListWidget::ObjectListWidget(QWidget *parent): QTableWidget(parent)
{
    setupUi();
}

ObjectListWidget::ObjectListWidget(bool editableObjects, QWidget *parent): QTableWidget(parent)
{
    setupUi();
    m_editableObjects = editableObjects;
}

void ObjectListWidget::setDragable(bool dragable)
{
    if (dragable)
    {
        this->setDragDropMode(InternalMove);
        this->setDragEnabled(true);
        this->setDefaultDropAction(Qt::MoveAction);
    }
    else
    {
        this->setDragDropMode(NoDragDrop);
        this->setDragEnabled(false);
        this->setDefaultDropAction(Qt::IgnoreAction);
    }
}

void ObjectListWidget::addObject(RamObject *obj, bool edit)
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
    default:
        ow = new RamObjectWidget(obj,this);
        break;
    }

    if (!m_editableObjects) ow->disableEdit();

    int row = this->rowCount();
    this->setRowCount( row + 1 );
    this->setCellWidget(row, 0, ow);
    this->setRowHeight(row, ow->size().height());

    if (m_editableObjects && edit) ow->edit();

    connect(obj, SIGNAL(removed(RamObject*)), this, SLOT(removeObject(RamObject*)));
}

QList<RamObject *> ObjectListWidget::objects() const
{
    QList<RamObject *> objs;
    for( int row = 0; row < this->rowCount(); row++)
    {
        // Get the object from the widget
        RamObjectWidget *ow = (RamObjectWidget*)this->cellWidget(row, 0 );
        if (ow) objs << ow->ramObject();
    }
    return objs;
}

void ObjectListWidget::removeObject(RamObject *obj)
{
    removeObject(obj->uuid());
}

void ObjectListWidget::removeObject(QString uuid)
{
    for( int row = 0; row < this->rowCount(); row++)
    {
        // Get the object from the widget
        RamObjectWidget *ow = (RamObjectWidget*)this->cellWidget(row, 0 );
        if (ow->ramObject()->uuid() == uuid)
        {
            ow->deleteLater();
            this->removeRow(row);
        }
    }
}

void ObjectListWidget::removeSelectedObjects()
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
            emit objectRemoved(ow->ramObject());
            ow->ramObject()->remove();
            ow->deleteLater();
        }
        this->removeRow(row);
    }
}

void ObjectListWidget::dropEvent(QDropEvent *event)
{
    QTableWidget::dropEvent(event);
    emit itemDropped();
}

void ObjectListWidget::resizeEvent(QResizeEvent *event)
{
    this->setColumnWidth( 0, event->size().width() );
    event->ignore();
}

void ObjectListWidget::itemSelected(QTableWidgetItem *previous, QTableWidgetItem *current)
{
    Q_UNUSED(previous);
    if (!current) return;
    RamObjectWidget *ow = (RamObjectWidget*)this->cellWidget( current->row(), 0 );
    if (!ow) return;
    emit objectSelected(ow->ramObject());
}

void ObjectListWidget::changeSelection()
{
    for(int row = 0; row < this->rowCount(); row++)
    {
        QTableWidgetItem *i = this->item(row, 0);
        RamObjectWidget *ow = (RamObjectWidget*) this->cellWidget( row, 0 );
        if (ow) ow->setSelected( i->isSelected() );
    }
}

void ObjectListWidget::updateOrder()
{
    for(int row = 0; row < this->rowCount(); row++)
    {
        RamObjectWidget *ow = (RamObjectWidget*) this->cellWidget( row, 0 );
        if (ow)
        {
            RamObject *o = ow->ramObject();
            if (o)
            {
                o->setOrder(row);
                o->update();
            }
        }
    }
    emit orderChanged();
}

void ObjectListWidget::setupUi()
{
    setDragable(true);
    this->setSelectionMode(ExtendedSelection);
    this->setFrameShape(QFrame::NoFrame);

    this->setColumnCount(1);
    this->setRowCount(0);

    this->verticalHeader()->setVisible(false);
    this->horizontalHeader()->setVisible(false);

    this->setColumnWidth( 0, this->size().width() );

    this->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
}

void ObjectListWidget::connectEvents()
{
    connect(this, &ObjectListWidget::itemDropped, this, &ObjectListWidget::updateOrder);
    connect(this, &QTableWidget::currentItemChanged, this, &ObjectListWidget::itemSelected);
    connect(this, &QTableWidget::itemSelectionChanged, this, &ObjectListWidget::changeSelection);
}
