#include "ramobjectlistwidget.h"

RamObjectListWidget::RamObjectListWidget(QWidget *parent):
        QTableView(parent)
{
    m_delegate = new RamObjectDelegate();
    setupUi();
    connectEvents();
}

RamObjectListWidget::RamObjectListWidget(RamObjectList *list, QWidget *parent):
    QTableView(parent)
{
    m_delegate = new RamObjectDelegate();
    setupUi();
    setList(list);
    connectEvents();
}


RamObjectListWidget::RamObjectListWidget(RamObjectList *list, bool editableObjects, RamUser::UserRole editRole, QWidget *parent):
    QTableView(parent)
{
    m_delegate = new RamObjectDelegate();
    m_delegate->setEditable(editableObjects);
    m_delegate->setEditRole(editRole);
    setupUi();
    setList(list);
    connectEvents();
}

void RamObjectListWidget::setList(RamObjectList *list)
{
    // We need to delete the previous selection model
    QItemSelectionModel *s = this->selectionModel();

    m_objectList = list;
    this->setModel(list);

    s->deleteLater();
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
    this->setColumnWidth( 0, event->size().width() );
}

void RamObjectListWidget::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous)

    quintptr iptr = current.data(Qt::UserRole).toULongLong();
    RamObject *obj = reinterpret_cast<RamObject*>( iptr) ;
    emit objectSelected(obj);
}

void RamObjectListWidget::search(QString s)
{
    if (!m_objectList) return;
    for (int i = 0; i < m_objectList->rowCount(); i++)
    {
        RamObject *obj = m_objectList->at(i);
        bool visible = s == "" || obj->name().contains(s, Qt::CaseInsensitive) || obj->shortName().contains(s, Qt::CaseInsensitive);
        this->verticalHeader()->setSectionHidden(i, !visible);
    }
}

void RamObjectListWidget::select(RamObject *o)
{
    if (!m_objectList) return;
    for (int i = 0; i< m_objectList->rowCount(); i++)
    {
        RamObject *obj = m_objectList->at(i);
        if (obj->is(o))
        {
            this->setCurrentIndex( m_objectList->index(i, 0));
        }
    }
}

void RamObjectListWidget::setupUi()
{
    this->setSelectionMode(QAbstractItemView::ExtendedSelection);
    this->setFrameShape(QFrame::NoFrame);
    this->setDragDropMode(QAbstractItemView::NoDragDrop);
    this->setDragEnabled(false);
    this->setDefaultDropAction(Qt::IgnoreAction);
    this->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    this->verticalHeader()->setSectionsMovable(false);
    this->setShowGrid(false);
    this->horizontalHeader()->hide();
    this->setColumnWidth( 0, this->size().width() );
    this->setMouseTracking(true);

    QString style = "QTableView { gridline-color: rgba(0,0,0,0); selection-background-color: rgba(0,0,0,0); } ";
    style += "QTableView::item:hover { background-color: none; } ";
    this->setStyleSheet(style);

    this->setItemDelegate( m_delegate );
}

void RamObjectListWidget::connectEvents()
{
    connect(m_delegate, &RamObjectDelegate::editObject, this, &RamObjectListWidget::editObject);
}

