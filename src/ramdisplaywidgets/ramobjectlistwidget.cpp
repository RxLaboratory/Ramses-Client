#include "ramobjectlistwidget.h"

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

RamObjectListWidget::RamObjectListWidget(QWidget *parent):
        QTableWidget(parent)
{
    setupUi();
    connectEvents();
    clear();
}

RamObjectListWidget::RamObjectListWidget(RamObjectList *list, QWidget *parent):
    QTableWidget(parent)
{
    setupUi();
    connectEvents();
    setList(list);
}

RamObjectListWidget::RamObjectListWidget(RamObjectUberList *list, QWidget *parent):
    QTableWidget(parent)
{
    setupUi();
    connectEvents();
    setList(list);
}

RamObjectListWidget::RamObjectListWidget(RamObjectList *list, bool editableObjects, QWidget *parent):
    QTableWidget(parent)
{
    m_editableObjects = editableObjects;
    setupUi();
    connectEvents();
    setList(list);
}

RamObjectListWidget::RamObjectListWidget(RamObjectUberList *list, bool editableObjects, QWidget *parent):
    QTableWidget(parent)
{
    m_editableObjects = editableObjects;
    setupUi();
    connectEvents();
    setList(list);
}

void RamObjectListWidget::setList(RamObjectList *list)
{
    clear();

    if (!list) return;

    addList(list);

    this->setEnabled(true);
}

void RamObjectListWidget::addList(RamObjectList *list)
{
    QSignalBlocker b(this);

    if (!list) return;
    m_lists << list;

    for (int i = 0; i < list->count(); i++) objectAssigned(list->at(i));

    m_listConnections << connect(list, &RamObjectList::objectRemoved, this, &RamObjectListWidget::objectUnassigned);
    m_listConnections << connect(list, &RamObjectList::objectAdded, this, &RamObjectListWidget::objectAssigned);
}

void RamObjectListWidget::setList(RamObjectUberList *list)
{
    clear();

    if (!list) return;

    for (int i = 0; i < list->count(); i++)
    {
        RamObjectList *sublist = qobject_cast<RamObjectList*>( list->at(i) );
        addList(sublist);
    }

    m_uberList = list;

    this->setEnabled(true);
}

void RamObjectListWidget::clear()
{
    this->setEnabled(false);

    m_uberList = nullptr;

    QSignalBlocker b(this);

    while ( !m_listConnections.isEmpty() ) disconnect( m_listConnections.takeLast() );
    m_lists.clear();
    this->setRowCount(0);
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


    if (m_editMode == RemoveObjects)
    {
        QMessageBox::StandardButton confirm = QMessageBox::question(this,
                                                                    "Confirm deletion",
                                                                    "Are you sure you want to premanently remove the selected items?" );

        if ( confirm != QMessageBox::Yes) return;

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
    else
    {
        for(int row =  this->rowCount() -1 ; row >= 0; row--)
        {
            QTableWidgetItem *i = this->item(row, 0);
            if (!i->isSelected()) continue;
            RamObjectWidget *ow = (RamObjectWidget*) this->cellWidget(row, 0);
            if (ow)
            {
                RamObject *o = ow->ramObject();
                for (int j = 0; j < m_lists.count(); j++)
                {
                    m_lists.at(j)->removeAll(o);
                }
            }
        }
    }

}

void RamObjectListWidget::search(QString nameOrShortName)
{
    QHeaderView *header = this->verticalHeader();
    for( int row = 0; row < this->rowCount(); row++)
    {
        if (nameOrShortName == "")
        {
            header->setSectionHidden(row, false);
            continue;
        }

        RamObjectWidget *ow = (RamObjectWidget*) this->cellWidget(row, 0);

        if (ow)
        {
            RamObject *o = ow->ramObject();
            if (o)
            {
                header->setSectionHidden( row,
                                          !o->shortName().contains(nameOrShortName, Qt::CaseInsensitive)
                                          && !o->name().contains(nameOrShortName, Qt::CaseInsensitive)
                                          );
                continue;
            }
        }

        header->setSectionHidden( row, this->item(row, 0)->text().contains(nameOrShortName) );
    }
}

void RamObjectListWidget::filter(QString uuid)
{
    QHeaderView *header = this->verticalHeader();
    for( int row = 0; row < this->rowCount(); row++)
    {
        if (uuid == "" || !m_uberList)
        {
            header->setSectionHidden(row, false);
            continue;
        }

        header->setSectionHidden( row, true );

        RamObjectWidget *ow = qobject_cast<RamObjectWidget*>( this->cellWidget(row, 0) );

        if (ow)
        {
            RamObject *o = ow->ramObject();
            if (o)
            {
                RamObjectList* sublist = qobject_cast<RamObjectList*>( m_uberList->fromUuid(uuid) );
                if (sublist)
                    header->setSectionHidden( row, !sublist->contains(o) );
            }
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
    if (!current) return;
    if (current == previous) return;
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
    case RamObject::Sequence:
    {
        RamSequence *s = dynamic_cast<RamSequence*>(obj);
        if (s) ow = new RamSequenceWidget(s, this);
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
    case RamObject::Project:
    {
        RamProject *p = dynamic_cast<RamProject*>(obj);
        if (p) ow = new RamProjectWidget(p, this);
        else ow = new RamObjectWidget(obj, this);
        break;
    }
    case RamObject::Step:
    {
        RamStep *s = dynamic_cast<RamStep*>(obj);
        if (s) ow = new RamStepWidget(s, this);
        else ow = new RamObjectWidget(obj, this);
        break;
    }
    case RamObject::AssetGroup:
    {
        RamAssetGroup *ag = dynamic_cast<RamAssetGroup*>(obj);
        if (ag) ow = new RamAssetGroupWidget(ag, this);
        else ow = new RamObjectWidget(obj, this);
        break;
    }
    case RamObject::FileType:
    {
        RamFileType *ft = dynamic_cast<RamFileType*>(obj);
        if (ft) ow = new RamFileTypeWidget(ft, this);
        else ow = new RamObjectWidget(obj, this);
        break;
    }
    case RamObject::Application:
    {
        RamApplication *a = dynamic_cast<RamApplication*>(obj);
        if (a) ow = new RamApplicationWidget(a, this);
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

void RamObjectListWidget::setEditMode(const EditMode &editMode)
{
    m_editMode = editMode;
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
    this->setShowGrid(false);

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
    connect(this->verticalHeader(), &QHeaderView::sectionMoved, this, &RamObjectListWidget::updateOrder);
    connect(this, &QTableWidget::currentItemChanged, this, &RamObjectListWidget::itemSelected);
    connect(this, &QTableWidget::itemSelectionChanged, this, &RamObjectListWidget::changeSelection);
}
