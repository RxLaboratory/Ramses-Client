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

    for (int i = 0; i < list->count(); i++)
    {
        RamObjectList *sublist = qobject_cast<RamObjectList*>( list->at(i) );
        addList(sublist);
    }

    m_stepType = stepType;

    for (int i = 0; i < steps->count(); i++) addStep( steps->at(i) );
    m_listConnections << connect(steps, &RamObjectList::objectRemoved, this, &ItemTableWidget::addStep);
    m_listConnections << connect(steps, &RamObjectList::objectAdded, this, &ItemTableWidget::removeStep);

    m_uberList = list;

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

    m_listConnections << connect(list, &RamObjectList::objectRemoved, this, &ItemTableWidget::objectUnassigned);
    m_listConnections << connect(list, &RamObjectList::objectAdded, this, &ItemTableWidget::objectAssigned);
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
    this->resizeRowToContents(row);

    // TODO Populate columns with (latest) status

    m_objectConnections[obj->uuid()] = connect(obj, &RamObject::changed, this, &ItemTableWidget::objectChanged);
}

void ItemTableWidget::setSortable(bool sortable)
{
    this->verticalHeader()->setVisible(sortable);
}

void ItemTableWidget::setupUi()
{
    this->setFrameShape(QFrame::NoFrame);
    this->setDragDropMode(NoDragDrop);
    this->setDragEnabled(false);
    this->setDefaultDropAction(Qt::IgnoreAction);
    this->verticalHeader()->setSectionsMovable(true);
    this->setShowGrid(false);

    this->setRowCount(0);
    this->setColumnCount(1);

    this->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

    int p = DuUI::getSize("padding", "large");
    QString padding = QString::number(p) + "px";
    QString style = "QTableWidget { gridline-color: rgba(0,0,0,0); padding-top:" + padding +  "; padding-bottom:" + padding +  "; selection-background-color: rgba(0,0,0,0); } ";
    style += "QTableWidget::item { padding-left: " + padding + "; padding-right: " + padding + "; } ";
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
        disconnect( m_objectConnections.take(obj->uuid()) );
    }
}

void ItemTableWidget::disconnectStep(QString stepUuid)
{
    if (m_stepConnections.contains( stepUuid ))
    {
        disconnect( m_stepConnections.take( stepUuid ) );
    }
}
