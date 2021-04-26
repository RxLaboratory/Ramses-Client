#include "objectlistwidget.h"

ObjectListWidget::ObjectListWidget(QWidget *parent): QTableWidget(parent)
{
    setupUi();
    connectEvents();
}

ObjectListWidget::ObjectListWidget(bool editableObjects, QWidget *parent): QTableWidget(parent)
{
    setupUi();
    connectEvents();
    m_editableObjects = editableObjects;
}

void ObjectListWidget::setSortable(bool sortable)
{
    this->verticalHeader()->setVisible(sortable);
}

void ObjectListWidget::setSelectable(bool selectable)
{
    if(selectable) this->setSelectionMode(ExtendedSelection);
    else this->setSelectionMode(NoSelection);
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
    this->setItem(row, 0, new QTableWidgetItem("   " + obj->name()));
    this->setCellWidget(row, 0, ow);
    this->resizeRowToContents(row);

    if (m_editableObjects && edit) ow->edit();

    connect(obj, SIGNAL(removed(RamObject*)), this, SLOT(removeObject(RamObject*)));
    connect(obj, &RamObject::changed, this, &ObjectListWidget::objectChanged);
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
            delete ow;
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

void ObjectListWidget::resizeEvent(QResizeEvent *event)
{
    this->setColumnWidth( 0, event->size().width() );
    this->setRowHeight(0, 10);
    this->resizeRowsToContents();
}

void ObjectListWidget::itemSelected(QTableWidgetItem *previous, QTableWidgetItem *current)
{
    Q_UNUSED(previous);
    if (!current) return;
    RamObjectWidget *ow = (RamObjectWidget*)this->cellWidget( current->row(), 0 );
    if (!ow) return;
    ow->setSelected(current->isSelected());
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
                o->setOrder(this->verticalHeader()->visualIndex(row));
                o->update();
            }
        }
    }
    emit orderChanged();
}

void ObjectListWidget::objectChanged(RamObject *obj)
{
    for( int row = 0; row < this->rowCount(); row++)
    {
        // Get the object from the widget
        RamObjectWidget *ow = (RamObjectWidget*)this->cellWidget(row, 0 );
        if (ow->ramObject()->uuid() == obj->uuid())
        {
            this->item(row, 0)->setText("   " + obj->name());
        }
    }
}

void ObjectListWidget::setupUi()
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

void ObjectListWidget::connectEvents()
{
    connect(this->verticalHeader(), &QHeaderView::sectionMoved, this, &ObjectListWidget::updateOrder);
    connect(this, &QTableWidget::currentItemChanged, this, &ObjectListWidget::itemSelected);
    connect(this, &QTableWidget::itemSelectionChanged, this, &ObjectListWidget::changeSelection);
}
