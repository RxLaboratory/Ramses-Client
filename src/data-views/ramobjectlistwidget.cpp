#include "ramobjectlistwidget.h"

RamObjectListWidget::RamObjectListWidget(DisplayMode mode, QWidget *parent):
        QTableView(parent)
{
    m_delegate = new RamObjectDelegate();
    m_displayMode = mode;
    setupUi();
    connectEvents();
    m_objectList = new RamObjectFilterModel(this);
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

}

RamObjectListWidget::RamObjectListWidget(RamObjectList *list, DisplayMode mode, QWidget *parent):
    QTableView(parent)
{
    m_delegate = new RamObjectDelegate();
    m_displayMode = mode;
    setupUi();
    connectEvents();
    m_objectList = new RamObjectFilterModel(this);
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
}

RamObjectListWidget::RamObjectListWidget(RamObjectList *list, bool editableObjects, RamUser::UserRole editRole, DisplayMode mode, QWidget *parent):
    QTableView(parent)
{
    m_displayMode = mode;
    m_delegate = new RamObjectDelegate();
    m_delegate->setEditable(editableObjects);
    m_delegate->setEditRole(editRole);
    setupUi();
    connectEvents();
    m_objectList = new RamObjectFilterModel(this);
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
}

void RamObjectListWidget::setList(RamObjectList *list)
{ 
    m_objectList->setList(list);
    this->resizeRowsToContents();
    this->resizeColumnsToContents();
}

void RamObjectListWidget::setEditableObjects(bool editableObjects, RamUser::UserRole editRole)
{
    m_delegate->setEditable(editableObjects);
    m_delegate->setEditRole(editRole);
}

void RamObjectListWidget::search(QString s)
{
    m_objectList->search(s);
    this->resizeRowsToContents();
}

void RamObjectListWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_dragging)
    {
        QPoint newPos = event->globalPos();
        QPoint _delta = newPos - _initialDragPos;
        this->horizontalScrollBar()->setValue( this->horizontalScrollBar()->value() - _delta.x() );
        this->verticalScrollBar()->setValue( this->verticalScrollBar()->value() - _delta.y() );
        _initialDragPos = newPos;
        event->accept();
        return;
    }
    QTableView::mouseMoveEvent(event);
}

void RamObjectListWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MiddleButton)
    {
        _initialDragPos = event->globalPos();
        m_dragging = true;
        event->accept();
        return;
    }
    QTableView::mousePressEvent(event);
}

void RamObjectListWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MiddleButton)
    {
        m_dragging = false;
        event->accept();
        return;
    }
    QTableView::mouseReleaseEvent(event);
}

void RamObjectListWidget::resizeEvent(QResizeEvent *event)
{
    if (m_displayMode == List) this->setColumnWidth( 0, event->size().width() );
    this->setRowHeight(0,10);
    if (m_displayMode == Table) this->setRowHeight(0,42);
    else this->setRowHeight(0,30);
}

void RamObjectListWidget::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous)

    quintptr iptr = current.data(Qt::UserRole).toULongLong();
    RamObject *obj = reinterpret_cast<RamObject*>( iptr) ;
    emit objectSelected(obj);
}

void RamObjectListWidget::select(RamObject *o)
{
    if (!m_objectList) return;
    for (int i = 0; i< m_objectList->rowCount(); i++)
    {
        quintptr iptr = m_objectList->data( m_objectList->index(i, 0), Qt::UserRole).toULongLong();
        RamObject *obj = reinterpret_cast<RamObject*>( iptr );
        if (obj->is(o))
        {
            this->setCurrentIndex( m_objectList->index(i, 0));
        }
    }
}

void RamObjectListWidget::filter(RamObject *o)
{
    if (!o) m_objectList->setFilterUuid("");
    else m_objectList->setFilterUuid(o->uuid());
    this->resizeRowsToContents();
}

void RamObjectListWidget::setupUi()
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

void RamObjectListWidget::connectEvents()
{
    connect(m_delegate, &RamObjectDelegate::editObject, this, &RamObjectListWidget::editObject);
    connect(m_delegate, &RamObjectDelegate::historyObject, this, &RamObjectListWidget::historyObject);
}

