#include "itemtablewidget.h"

#include "ramassetwidget.h"
#include "ramshotwidget.h"
#include "ramstatuswidget.h"
#include "ramstatewidget.h"
#include "ramuserwidget.h"
#include "ramprojectwidget.h"
#include "ramstepwidget.h"
#include "ramassetgroupwidget.h"
#include "ramfiletypewidget.h"
#include "ramapplicationwidget.h"
#include "ramsequencewidget.h"

ItemTableWidget::ItemTableWidget(QWidget *parent):
    QTableWidget(parent)
{
    setupUi();
    connectEvents();
    clear();
}

void ItemTableWidget::setList(RamObjectUberList *list, RamObjectList *steps, RamStep::Type stepType)
{
    clear();

    if (!list) return;

    for (int i = 0; i < steps->count(); i++) addStep( steps->at(i) );
    m_listConnections << connect(steps, &RamObjectList::objectRemoved, this, &ItemTableWidget::addStep);
    m_listConnections << connect(steps, &RamObjectList::objectAdded, this, &ItemTableWidget::removeStep);

    for (int i = 0; i < list->count(); i++)
    {
        RamObjectList *sublist = qobject_cast<RamObjectList*>( list->at(i) );
        addList(sublist);
    }

    m_stepType = stepType;

    m_uberList = list;

    if( stepType == RamStep::AssetProduction ) this->horizontalHeaderItem(0)->setText("Assets");
    else this->horizontalHeaderItem(0)->setText("Shots");

    this->setEnabled(true);
}

void ItemTableWidget::clear()
{
    this->setEnabled(false);

    m_uberList = nullptr;

    QSignalBlocker b(this);

    while ( !m_listConnections.isEmpty() ) disconnect( m_listConnections.takeLast() );
    m_lists.clear();

    // Disconnect all objects and steps
    for( int row = 0; row < this->rowCount(); row++)
    {
        RamObjectWidget *ow = qobject_cast<RamObjectWidget*>( this->cellWidget(row, 0 ) );
        disconnectObject( ow->ramObject() );
    }

    for( int col = 1; col < this->columnCount(); col++)
    {
        QString stepUuid = this->horizontalHeaderItem(col)->data(Qt::UserRole).toString();
        disconnectStep( stepUuid );
    }

    this->setRowCount(0);
    this->setColumnCount(1);
}

void ItemTableWidget::addList(RamObjectList *list)
{
    QSignalBlocker b(this);

    if (!list) return;
    m_lists << list;

    for (int i = 0; i < list->count(); i++) objectAssigned(list->at(i));

    this->resizeColumnToContents(0);

    m_listConnections << connect(list, &RamObjectList::objectRemoved, this, &ItemTableWidget::objectUnassigned);
    m_listConnections << connect(list, &RamObjectList::objectAdded, this, &ItemTableWidget::objectAssigned);
}

void ItemTableWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (_dragging)
    {
        QPoint newPos = event->globalPos();
        QPoint _delta = newPos - _initialDragPos;
        this->horizontalScrollBar()->setValue( this->horizontalScrollBar()->value() - _delta.x() );
        this->verticalScrollBar()->setValue( this->verticalScrollBar()->value() - _delta.y() );
        _initialDragPos = newPos;
        event->accept();
        return;
    }
    QTableWidget::mouseMoveEvent(event);
}

void ItemTableWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MiddleButton)
    {
        _initialDragPos = event->globalPos();
        _dragging = true;
        event->accept();
        return;
    }
    QTableWidget::mousePressEvent(event);
}

void ItemTableWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MiddleButton)
    {
        _dragging = false;
        event->accept();
        return;
    }
    QTableWidget::mouseReleaseEvent(event);
}

void ItemTableWidget::addStep(RamObject *stepObj)
{
    RamStep *step = qobject_cast<RamStep*>( stepObj );
    if (step->type() != m_stepType) return;

    int col = this->columnCount();

    this->setColumnCount(col + 1);

    QTableWidgetItem *stepItem = new QTableWidgetItem(stepObj->name());
    stepItem->setData(Qt::UserRole, stepObj->uuid());

    this->setHorizontalHeaderItem(col,stepItem);

    m_stepConnections[stepObj->uuid()] = connect(stepObj, &RamObject::changed, this, &ItemTableWidget::stepChanged);
}

void ItemTableWidget::removeStep(RamObject *stepObj)
{
    disconnectStep( stepObj->uuid() );

    for (int i = this->columnCount() -1; i > 0; i--)
    {
        QString colUuid = this->horizontalHeaderItem(i)->data(Qt::UserRole).toString();
        if (colUuid == stepObj->uuid())
        {
            this->removeColumn(i);
            return;
        }
    }
}

void ItemTableWidget::stepChanged(RamObject *stepObj)
{
    for (int i = this->columnCount() -1; i > 0; i--)
    {
        QString colUuid = this->horizontalHeaderItem(i)->data(Qt::UserRole).toString();
        if (colUuid == stepObj->uuid())
        {
            this->horizontalHeaderItem(i)->setText(stepObj->name());
            return;
        }
    }
}

void ItemTableWidget::statusAdded(RamObject *statusObj, int index)
{
    Q_UNUSED(index);
    RamStatus *status = qobject_cast<RamStatus*>( statusObj );
    if (!status) return;
    RamItem *item = status->item();
    RamStep *step = status->step();

    //Sort history
    RamStepStatusHistory *statusHistory = qobject_cast<RamStepStatusHistory*>( sender() );
    statusHistory->sort();

    setStatusWidget(item, step, status);
}

void ItemTableWidget::statusRemoved(RamObject *statusObj)
{
    RamStatus *status = qobject_cast<RamStatus*>( statusObj );
    if (!status) return;
    RamItem *item = status->item();
    RamStep *step = status->step();

    setStatusWidget(item, step, nullptr);
}

void ItemTableWidget::stepHistoryAdded(RamObject *statusHistoryObj)
{
    // Get item and step
    RamStepStatusHistory *statusHistory = qobject_cast<RamStepStatusHistory*>( statusHistoryObj );
    if (!statusHistory) return;
    RamItem *item = statusHistory->item();
    RamStep *step = statusHistory->step();

    statusHistory->sort();

    RamStatus *status = qobject_cast<RamStatus*>( statusHistory->last() );

    setStatusWidget(item, step, status);

    QList<QMetaObject::Connection> c = m_objectConnections.value( item->uuid() );
    c << connect(statusHistory, &RamObjectList::objectAdded, this, &ItemTableWidget::statusAdded);
    c << connect(statusHistory, &RamObjectList::objectRemoved, this, &ItemTableWidget::statusRemoved);
    m_objectConnections[item->uuid()] = c;
}

void ItemTableWidget::objectChanged(RamObject *obj)
{
    for( int row = 0; row < this->rowCount(); row++)
    {
        // Get the object from the widget
        RamObjectWidget *ow = qobject_cast<RamObjectWidget*>( this->cellWidget(row, 0 ) );
        if (ow->ramObject()->is(obj))
        {
            this->item(row, 0)->setText("   " + obj->name());
        }
    }
}

void ItemTableWidget::objectUnassigned(RamObject *obj)
{
    disconnectObject(obj);

    for( int row = 0; row < this->rowCount(); row++)
    {
        // Get the object from the widget
        RamObjectWidget *ow = qobject_cast<RamObjectWidget*>( this->cellWidget(row, 0 ) );
        if (ow->ramObject()->is(obj))
        {
            delete ow;
            this->removeRow(row);
        }
    }
}

void ItemTableWidget::objectAssigned(RamObject *obj)
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
        return;
    }

    //ow->setEditable(m_editableObjects);

    int row = this->rowCount();
    this->setRowCount( row + 1 );
    this->setItem(row, 0, new QTableWidgetItem("   " + obj->name()));
    this->setCellWidget(row, 0, ow);
    QTableWidgetItem *headerItem = new QTableWidgetItem(obj->name());
    headerItem->setData(Qt::UserRole, obj->uuid());
    this->setVerticalHeaderItem(row, headerItem);

    RamItem *item = qobject_cast<RamItem*>( obj );
    for (int col = 0; col < this->columnCount(); col++)
    {
        QString stepUuid = this->horizontalHeaderItem(col)->data(Qt::UserRole).toString();
        RamStepStatusHistory *statusHistory = item->statusHistory(stepUuid);
        RamStatus *status = nullptr;
        if (statusHistory)
        {
            statusHistory->sort();
            status = qobject_cast<RamStatus*>( statusHistory->last() );
        }
        else
        {
            RamProject *p = Ramses::instance()->currentProject();
            RamStep *st = nullptr;
            if (p) st = qobject_cast<RamStep*>( p->steps()->fromUuid(stepUuid) );
            status = generateDefaultStatus(item, st);
        }
        if (status)
        {
            RamStatusWidget *sw = new RamStatusWidget(status, this);
            sw->setEditable(false);

            this->setCellWidget(row, col, sw);
        }
    }

    this->resizeRowToContents(row);

    // Connect status history (added, removed)
    QList<QMetaObject::Connection> c;
    c << connect(item->statusHistory(), &RamObjectList::objectAdded, this, &ItemTableWidget::stepHistoryAdded);
    c << connect(item->statusHistory(), &RamObjectUberList::objectAdded, this, &ItemTableWidget::statusAdded);
    c << connect(item->statusHistory(), &RamObjectUberList::objectRemoved, this, &ItemTableWidget::statusRemoved);
    c << connect(obj, &RamObject::changed, this, &ItemTableWidget::objectChanged);
    m_objectConnections[obj->uuid()] = c;
}

void ItemTableWidget::setSortable(bool sortable)
{
    this->verticalHeader()->setSectionsMovable(sortable);
}

void ItemTableWidget::setupUi()
{
    this->setFrameShape(QFrame::NoFrame);
    this->setDragDropMode(NoDragDrop);
    this->setDragEnabled(false);
    this->setDefaultDropAction(Qt::IgnoreAction);  
    this->setShowGrid(false);

    this->setRowCount(0);
    this->setColumnCount(1);

    this->setHorizontalHeaderItem(0, new QTableWidgetItem("Item"));

    this->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    this->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);

    this->horizontalHeader()->setSectionsMovable(true);

    this->setAlternatingRowColors(true);

    int p = DuUI::getSize("padding", "large");
    int sp = DuUI::getSize("margin");
    QString padding = QString::number(p) + "px";
    QString smallPadding = QString::number(sp) + "px";
    QString style = "QTableWidget { gridline-color: rgba(0,0,0,0); selection-background-color: rgba(0,0,0,0); } ";
    style += "QTableWidget::item { padding-left: " + padding + "; padding-right: " + padding + "; padding-top: " + smallPadding + "; } ";
    style += "QTableWidget::item:hover { background-color: none; } ";

    this->setStyleSheet(style);
}

void ItemTableWidget::connectEvents()
{

}

void ItemTableWidget::disconnectObject(RamObject *obj)
{
    if (m_objectConnections.contains(obj->uuid()))
    {
        QList<QMetaObject::Connection> c = m_objectConnections.take(obj->uuid());
        while (!c.isEmpty()) disconnect( c.takeLast() );
    }
}

void ItemTableWidget::disconnectStep(QString stepUuid)
{
    if (m_stepConnections.contains( stepUuid ))
    {
        disconnect( m_stepConnections.take( stepUuid ) );
    }
}

void ItemTableWidget::setStatusWidget(RamItem *item, RamStep *step, RamStatus *status)
{

    // Get row & col
    for (int row = 0; row < this->rowCount(); row++)
    {
        if (this->verticalHeaderItem(row)->data(Qt::UserRole).toString() == item->uuid() )
        {
            for (int col = 0; col < this->colorCount(); col++)
            {
                if (this->horizontalHeaderItem(col)->data(Qt::UserRole).toString() == step->uuid() )
                {
                    // Remove current status widget
                    QWidget *csw = this->cellWidget(row, col);
                    if (csw) delete csw;
                    // Add the new one
                    if (!status) status =  generateDefaultStatus(item, step);
                    if (status)
                    {
                        RamStatusWidget *sw = new RamStatusWidget(status, this);
                        this->setCellWidget(row, col, sw);
                    }
                    return;
                }
            }
        }
    }
}

RamStatus *ItemTableWidget::generateDefaultStatus(RamItem *item, RamStep *step)
{
    RamStatus *status = nullptr;
    RamUser *u = Ramses::instance()->ramUser();
    if (!u) u = Ramses::instance()->currentUser();
    RamState *s = Ramses::instance()->noState();
    if (u && s && step) status = new RamStatus( u, s, step, item );
    return status;
}
