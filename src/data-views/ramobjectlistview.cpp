#include "ramobjectlistview.h"

#include "data-models/ramitemtablelistproxy.h"
#include "timelinemanager.h"

template<typename RO>
RamObjectListView<RO>::RamObjectListView(DisplayMode mode, QWidget *parent):
        QTableView(parent)
{
    m_delegate = new RamObjectDelegate<RO>();
    m_displayMode = mode;
    setupUi();
    m_objectList = new RamItemFilterModel(this);
    if (mode == List)
    {
        RamItemTableListProxy *tlp = new RamItemTableListProxy(this);
        tlp->setSourceModel(m_objectList);
        this->setModel(tlp);
    }
    else
    {
        this->setModel(m_objectList);
    }
    connectEvents();
}

template<typename RO>
RamObjectListView<RO>::RamObjectListView(RamObjectList<RO> *list, DisplayMode mode, QWidget *parent):
    QTableView(parent)
{
    m_delegate = new RamObjectDelegate<RO>();
    m_displayMode = mode;
    setupUi();
    m_objectList = new RamItemFilterModel(this);
    if (mode == List)
    {
        RamItemTableListProxy *tlp = new RamItemTableListProxy(this);
        tlp->setSourceModel(m_objectList);
        this->setModel(tlp);
    }
    else
    {
        this->setModel(m_objectList);
    }
    this->setList(list);
    connectEvents();
}

template<typename RO>
RamObjectListView<RO>::RamObjectListView(RamObjectList<RO> *list, bool editableObjects, RamUser::UserRole editRole, DisplayMode mode, QWidget *parent):
    QTableView(parent)
{
    m_displayMode = mode;
    m_delegate = new RamObjectDelegate<RO>();
    m_delegate->setEditable(editableObjects);
    m_delegate->setEditRole(editRole);
    setupUi();
    m_objectList = new RamItemFilterModel(this);
    if (mode == List)
    {
        RamItemTableListProxy *tlp = new RamItemTableListProxy(this);
        tlp->setSourceModel(m_objectList);
        this->setModel(tlp);
    }
    else
    {
        this->setModel(m_objectList);
    }
    setList(list);
    connectEvents();
}

template<typename RO>
void RamObjectListView<RO>::setList(RamObjectList<RO> *list)
{
    m_objectList->setList(list);
    this->resizeRowsToContents();
    this->resizeColumnsToContents();
}

template<typename RO>
RamItemFilterModel *RamObjectListView<RO>::filteredList()
{
    return m_objectList;
}

template<typename RO>
void RamObjectListView<RO>::setEditableObjects(bool editableObjects, RamUser::UserRole editRole)
{
    m_delegate->setEditable(editableObjects);
    m_delegate->setEditRole(editRole);
}

template<typename RO>
void RamObjectListView<RO>::setSortable(bool sortable)
{
    this->verticalHeader()->setSectionsMovable(sortable);
}

template<typename RO>
void RamObjectListView<RO>::setTimeTracking(bool trackTime)
{
    m_delegate->setTimeTracking(trackTime);
}

template<typename RO>
void RamObjectListView<RO>::setCompletionRatio(bool showCompletion)
{
    m_delegate->setCompletionRatio(showCompletion);
}

template<typename RO>
void RamObjectListView<RO>::showDetails(bool s)
{
    m_delegate->showDetails(s);
    this->resizeRowsToContents();
}

template<typename RO>
void RamObjectListView<RO>::search(QString s)
{
    // Search
    m_objectList->search(s);
    //this->resizeRowsToContents();
}

template<typename RO>
void RamObjectListView<RO>::mouseMoveEvent(QMouseEvent *event)
{
    if (m_dragging)
    {
        QPoint newPos = event->globalPos();
        QPoint _delta = newPos - m_initialDragPos;
        this->horizontalScrollBar()->setValue( this->horizontalScrollBar()->value() - _delta.x() );
        this->verticalScrollBar()->setValue( this->verticalScrollBar()->value() - _delta.y() );
        m_initialDragPos = newPos;
        event->accept();
        return;
    }
    QTableView::mouseMoveEvent(event);
}

template<typename RO>
void RamObjectListView<RO>::mousePressEvent(QMouseEvent *event)
{
    // Middle click for dragging view
    if (event->button() == Qt::MiddleButton)
    {
        m_initialDragPos = event->globalPos();
        m_dragging = true;
        event->accept();
        return;
    }
    // Simple click detection
    else if (event->button() == Qt::LeftButton && event->modifiers().testFlag(Qt::NoModifier))
    {
        // Get the index
        m_clicking = this->indexAt( event->localPos().toPoint() );
    }
    QTableView::mousePressEvent(event);
}

template<typename RO>
void RamObjectListView<RO>::mouseReleaseEvent(QMouseEvent *event)
{
    // Middle click for dragging view
    if (event->button() == Qt::MiddleButton)
    {
        m_dragging = false;
        event->accept();
        return;
    }
    // Simple click to select
    else if (m_clicking.isValid())
    {
        // Check index
        QModelIndex testIndex = this->indexAt( event->localPos().toPoint() );
        if (testIndex == m_clicking && event->modifiers().testFlag(Qt::NoModifier)) select(m_clicking);
        // Release
        m_clicking = QModelIndex();
    }
    QTableView::mouseReleaseEvent(event);
}

template<typename RO>
void RamObjectListView<RO>::resizeEvent(QResizeEvent *event)
{
    if (m_displayMode == List) this->setColumnWidth( 0, event->size().width() );
    // Incorrect draw after resize, fixed by resizing the first row...
    this->setRowHeight(0,30);
    this->resizeRowToContents(0);
    /*this->setRowHeight(0,10);
    if (m_displayMode == Table) this->setRowHeight(0,42);
    else this->setRowHeight(0,30);*/
}

template<typename RO>
void RamObjectListView<RO>::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)

    if(m_layout) return;
    this->resizeRowsToContents();
    this->resizeColumnsToContents();
    m_layout = true;
}

template<typename RO>
void RamObjectListView<RO>::select(const QModelIndex &index)
{
    RO obj = m_objectList->at(index);
    if (!obj) return;
    this->selectionModel()->select(index, QItemSelectionModel::ClearAndSelect);
    emit objectSelected(obj);
}

template<>
void RamObjectListView<RamShot *>::select(const QModelIndex &index)
{
    RamShot *obj = qobject_cast<RamShot *>( m_objectList->at(index) );
    if (!obj) return;
    this->selectionModel()->select(index, QItemSelectionModel::ClearAndSelect);
    emit objectSelected(obj);
}

template<typename RO>
void RamObjectListView<RO>::rowMoved(int logicalIndex, int oldVisualIndex, int newVisualIndex)
{
    Q_UNUSED(logicalIndex);

    QSignalBlocker b(this->verticalHeader());

    //m_objectList->moveRow(QModelIndex(), oldVisualIndex, QModelIndex(), newVisualIndex);

    // Get the source model to move the row
    QAbstractItemModel *model = m_objectList->sourceModel();
    // Convert the filtered index to the model index
    QModelIndex oldIndex = m_objectList->index(oldVisualIndex, 0);
    QModelIndex newIndex = m_objectList->index(newVisualIndex, 0);
    oldIndex = m_objectList->mapToSource(oldIndex);
    newIndex = m_objectList->mapToSource(newIndex);
    model->moveRow(QModelIndex(), oldIndex.row(), QModelIndex(), newIndex.row());//*/

    // move back to the (new) logical index
    this->verticalHeader()->moveSection(newVisualIndex, oldVisualIndex);
}

template<typename RO>
void RamObjectListView<RO>::revealFolder(RamObject *obj)
{
    obj->revealFolder();
}

template<typename RO>
void RamObjectListView<RO>::select(RO o)
{
    if (!m_objectList) return;
    for (int i = 0; i< m_objectList->rowCount(); i++)
    {
        RO obj = m_objectList->at(i);
        if (obj->is(o))
        {
            QModelIndex index = m_objectList->index(i, 0);
            this->setCurrentIndex( index );
            this->select( index );
            this->scrollTo(index, QAbstractItemView::PositionAtTop);
            RamShot *shot = reinterpret_cast<RamShot*>( obj );
            if (shot) TimelineManager::instance()->setCurrentShot(shot);
            return;
        }
    }
}

template<>
void RamObjectListView<RamShot*>::select(RamShot *o)
{
    if (!m_objectList) return;
    for (int i = 0; i< m_objectList->rowCount(); i++)
    {
        RamItem *obj = m_objectList->at(i);
        if (obj->is(o))
        {
            QModelIndex index = m_objectList->index(i, 0);
            this->setCurrentIndex( index );
            this->select( index );
            this->scrollTo(index, QAbstractItemView::PositionAtTop);
            TimelineManager::instance()->setCurrentShot(o);
            return;
        }
    }
}

template<typename RO>
void RamObjectListView<RO>::filter(RO o)
{
    if (!o) m_objectList->setFilterUuid("");
    else m_objectList->setFilterUuid(o->uuid());
    this->resizeRowsToContents();
}

template<typename RO>
void RamObjectListView<RO>::setupUi()
{
    this->setSelectionMode(QAbstractItemView::ExtendedSelection);
    this->setFrameShape(QFrame::NoFrame);
    this->setDragDropMode(QAbstractItemView::NoDragDrop);
    this->setDragEnabled(false);
    this->setDefaultDropAction(Qt::IgnoreAction);
    this->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    this->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    this->verticalHeader()->setSectionsMovable(false);
    this->setShowGrid(false);
    if (m_displayMode == List) this->horizontalHeader()->hide();
    this->setColumnWidth( 0, this->size().width() );
    this->setMouseTracking(true);

    QString style = "QTableView { padding-top: 3px; padding-bottom: 3px; gridline-color: rgba(0,0,0,0); selection-background-color: rgba(0,0,0,0); } ";
    style += "QTableView::item:hover { background-color: none; } ";
    this->setStyleSheet(style);

    this->setItemDelegate( m_delegate );
}

template<typename RO>
void RamObjectListView<RO>::connectEvents()
{
    connect(m_delegate, &RamObjectDelegate<RO>::editObject, this, &RamObjectListView::editObject);
    connect(m_delegate, SIGNAL(editObject(RamObject*)), this, SLOT(select(RamObject*)));
    connect(m_delegate, &RamObjectDelegate<RO>::historyObject, this, &RamObjectListView::historyObject);
    connect(m_delegate, &RamObjectDelegate<RO>::folderObject, this, &RamObjectListView::revealFolder);
    // SORT
    connect( this->verticalHeader(), SIGNAL(sectionMoved(int,int,int)), this, SLOT(rowMoved(int,int,int)));
    // SELECT
    // Unselect before filtering
    connect(m_objectList, SIGNAL(aboutToFilter()), this->selectionModel(), SLOT(clear()));
    connect(TimelineManager::instance(), SIGNAL(currentShotChanged(RamShot*)), this, SLOT(selectShot(RamShot*)));
}
