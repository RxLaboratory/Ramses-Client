#include "ramobjectlistwidget_bak.h"

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
#include "rampipefilewidget.h"

RamObjectListWidget::RamObjectListWidget(QWidget *parent):
        QTableWidget(parent)
{
    setupUi();
    connectEvents();

    clear();
    m_ready = true;
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

void RamObjectListWidget::setContainingType(RamObject::ObjectType type)
{
    m_containingType = type;
}

void RamObjectListWidget::setList(RamObjectList *list)
{
    m_list = list;
    m_uberList = nullptr;

    m_ready = false;
    if (this->isVisible()) addLists();
}

void RamObjectListWidget::setList(RamObjectUberList *list)
{
    m_list = nullptr;
    m_uberList = list;

    m_ready = false;
    if (this->isVisible()) addLists();
}

void RamObjectListWidget::clear()
{
    this->setEnabled(false);

    QSignalBlocker b(this);

    // Disconnect list
    while ( !m_listConnections.isEmpty() ) disconnect( m_listConnections.takeLast() );

    // Disconnect all objects
    QMapIterator<QString, QMetaObject::Connection> i( m_objectConnections );
    while(i.hasNext())
    {
        i.next();
        disconnect( i.value() );
    }
    m_objectConnections.clear();

    this->setRowCount(0);
}

void RamObjectListWidget::setSortable(bool sortable)
{
    this->verticalHeader()->setSectionsMovable(sortable);
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
        if (obj->uuid() == objUuid(row))
            this->item(row, 0)->setSelected(true);
    }
}

void RamObjectListWidget::removeSelectedObjects()
{
    if (this->selectedItems().count() == 0 ) return;


    if (m_editMode == RemoveObjects)
    {
        QMessageBox::StandardButton confirm = QMessageBox::question( this,
            "Confirm deletion",
            "Are you sure you want to premanently remove the selected items?" );

        if ( confirm != QMessageBox::Yes) return;

        for(int row =  this->rowCount() -1 ; row >= 0; row--)
        {
            QTableWidgetItem *i = this->item(row, 0);
            if (!i->isSelected()) continue;

            RamObject *o = objAtRow(row);
            if (o) o->remove();
        }
    }
    else
    {
        for(int row =  this->rowCount() -1 ; row >= 0; row--)
        {
            QTableWidgetItem *i = this->item(row, 0);
            if (!i->isSelected()) continue;

            QString uuid = objUuid(row);
            if (m_uberList) m_uberList->removeObject( uuid );
            else if (m_list) m_list->removeAll( uuid );
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

        // short name
        if (objShortName(row).contains(nameOrShortName, Qt::CaseInsensitive))
        {
            header->setSectionHidden(row, false);
            continue;
        }

        if (objName(row).contains(nameOrShortName, Qt::CaseInsensitive))
        {
            header->setSectionHidden(row, false);
            continue;
        }

        header->setSectionHidden(row, true);
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

        RamObjectList* sublist = RamObjectList::objectList( uuid );
        if (sublist)
        {
            header->setSectionHidden( row, !sublist->contains( objUuid(row) ) );
        }
    }
}

void RamObjectListWidget::mouseMoveEvent(QMouseEvent *event)
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

void RamObjectListWidget::mousePressEvent(QMouseEvent *event)
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

void RamObjectListWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MiddleButton)
    {
        _dragging = false;
        event->accept();
        return;
    }
    QTableWidget::mouseReleaseEvent(event);
}

void RamObjectListWidget::resizeEvent(QResizeEvent *event)
{
    this->setColumnWidth( 0, event->size().width() );
    this->setRowHeight(0, 10);
    this->resizeRowsToContents();
}

void RamObjectListWidget::showEvent(QShowEvent *event)
{
    addLists();
    QWidget::showEvent(event);
}

void RamObjectListWidget::itemSelected(QTableWidgetItem *current, QTableWidgetItem *previous)
{
    if (!current) return;
    if (current == previous) return;

    RamObjectWidget *ow = (RamObjectWidget*)this->cellWidget( current->row(), 0 );
    if (!ow) return;
    ow->setSelected(current->isSelected());
    RamObject *obj = ow->ramObject();
    if (!obj) return;
    emit objectSelected( obj );
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
    m_sorting = true;
    for(int row = 0; row < this->rowCount(); row++)
    {
        RamObject *o = objAtRow(row);
        if (o)
        {
            o->setOrder(this->verticalHeader()->visualIndex(row));
            o->update();
        }
    }
    if (m_list) m_list->sort();
    if (m_uberList) m_uberList->sortLists();

    emit orderChanged();
    m_sorting = false;
}

void RamObjectListWidget::listSorted()
{
    if (m_sorting) return;
    // Reload
    if(m_list) setList(m_list);
    if (m_uberList) setList(m_uberList);
}

void RamObjectListWidget::objectChanged(RamObject *obj)
{
    int row = objRow(obj);
    if (row < 0) return;
    this->item(row, 0)->setText("    " + obj->name());
    this->item(row, 0)->setData(Qt::UserRole, obj->order());
    this->verticalHeaderItem(row)->setText(obj->shortName());
}

void RamObjectListWidget::objectUnassigned(RamObject *obj)
{
    if (m_objectConnections.contains(obj->uuid()))
    {
        disconnect( m_objectConnections.take(obj->uuid()) );
    }

    int row = objRow(obj);
    this->removeRow(row);
}

void RamObjectListWidget::objectAssigned(RamObject *obj)
{
    // Check type to create widget
    RamObject::ObjectType type = obj->objectType();
    RamObjectWidget *ow;

    if (m_containingType != RamObject::Generic)
        if (type != m_containingType) return;

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
    case RamObject::PipeFile:
    {
        RamPipeFile *pf = dynamic_cast<RamPipeFile*>(obj);
        if (pf) ow = new RamPipeFileWidget(pf, this);
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
    RamObjectTableItem *item = new RamObjectTableItem(obj);
    this->setItem(row, 0, item);
    this->setCellWidget(row, 0, ow);
    QTableWidgetItem *header = new QTableWidgetItem(obj->shortName());
    header->setData(Qt::UserRole, obj->uuid() );
    this->setVerticalHeaderItem(row, header);
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
    this->setSelectionMode(NoSelection);
    this->setFrameShape(QFrame::NoFrame);
    this->setDragDropMode(NoDragDrop);
    this->setDragEnabled(false);
    this->setDefaultDropAction(Qt::IgnoreAction);
    this->verticalHeader()->setSectionsMovable(false);
    this->setShowGrid(false);

    this->setColumnCount(1);
    this->setRowCount(0);

    this->horizontalHeader()->setVisible(false);

    this->setColumnWidth( 0, this->size().width() );

    this->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

    int p = DuUI::getSize("padding", "large");
    int sp = DuUI::getSize("margin");
    QString padding = QString::number(p) + "px";
    QString smallPadding = QString::number(sp) + "px";
    QString style = "QTableWidget { gridline-color: rgba(0,0,0,0); padding-top:" + smallPadding +  "; selection-background-color: rgba(0,0,0,0); } ";
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

QString RamObjectListWidget::objUuid(int row)
{
    return this->verticalHeaderItem(row)->data(Qt::UserRole).toString();
}

QString RamObjectListWidget::objShortName(int row)
{
    return this->verticalHeaderItem(row)->text();
}

QString RamObjectListWidget::objName(int row)
{
    return this->item(row, 0)->text().trimmed();
}

RamObject *RamObjectListWidget::objAtRow(int row)
{
    return RamObject::obj( objUuid(row) );
}

int RamObjectListWidget::objRow(RamObject *o)
{
    for(int row = 0; row < this->rowCount(); row++)
    {
        if (objUuid(row) == o->uuid())
        {
            return row;
        }
    }
    return -1;
}

void RamObjectListWidget::addLists()
{
    if (m_ready) return;
    m_ready = true;

    clear();

    if (!m_uberList && !m_list) return;

    QSignalBlocker b(this);

    ProcessManager *pm = ProcessManager::instance();
    pm->start();

    if (m_uberList)
    {
        int count = m_uberList->objectCount();
        pm->setMaximum( count );
        pm->setText("Loading " + m_uberList->name() + "...");

        for (int i = 0; i < count; i++)
        {
            pm->increment();
            objectAssigned( m_uberList->objectAt(i) );
        }

        // Connect
        m_listConnections << connect(m_uberList, SIGNAL(objectAdded(RamObject*,int)), this, SLOT(objectAssigned(RamObject*)));
        m_listConnections << connect(m_uberList, SIGNAL(objectRemoved(RamObject*)), this, SLOT(objectUnassigned(RamObject*)));
        m_listConnections << connect(m_uberList, SIGNAL(sorted()), this, SLOT(listSorted()));
    }
    else if (m_list)
    {
        int count = m_list->count();
        pm->setMaximum( count );
        pm->setText("Loading " + m_list->name() + "...");

        for (int i = 0; i < count; i++)
        {
            pm->increment();
            objectAssigned( m_list->at(i) );

            // Connect
            m_listConnections << connect(m_list, SIGNAL(objectAdded(RamObject*,int)), this, SLOT(objectAssigned(RamObject*)));
            m_listConnections << connect(m_list, SIGNAL(objectRemoved(RamObject*)), this, SLOT(objectUnassigned(RamObject*)));
            m_listConnections << connect(m_list, SIGNAL(sorted()), this, SLOT(listSorted()));
        }
    }

    this->setEnabled(true);
    pm->finish();
}

RamObjectTableItem::RamObjectTableItem(RamObject *o): QTableWidgetItem()
{
    this->setText("    " + o->name());
    this->setData(Qt::UserRole, o->order());
}

bool RamObjectTableItem::operator<(const QTableWidgetItem &other) const
{
    return this->data(Qt::UserRole).toInt() < other.data(Qt::UserRole).toInt();
}

