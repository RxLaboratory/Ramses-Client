#include "ramobjectview.h"

#include "qapplication.h"
#include "qclipboard.h"
#include "qdir.h"
#include "qevent.h"
#include "qmessagebox.h"
#include "ramobjectsortfilterproxymodel.h"
#include "timelinemanager.h"
#include "duapp/duui.h"
#include "duwidgets/duicon.h"

RamObjectView::RamObjectView(DisplayMode mode, QWidget *parent):
    DuTableView(parent)
{
    m_displayMode = mode;
    setupUi();
    connectEvents();
}

void RamObjectView::setContextMenuDisabled(bool disabled)
{
    m_contextMenuDisabled = disabled;
}

void RamObjectView::setObjectModel(QAbstractItemModel *model)
{
    m_objectModel->freeze();
    m_objectModel->setSourceModel(model);
    this->setModel(m_objectModel);
    m_objectModel->unFreeze();
    //this->resizeRowsToContents();
    //this->resizeColumnsToContents();
}

QAbstractItemModel *RamObjectView::objectModel() const
{
    return m_objectModel->sourceModel();
}

void RamObjectView::setEditableObjects(bool editableObjects, RamUser::UserRole editRole)
{
    m_delegate->setEditable(editableObjects);
    m_delegate->setEditRole(editRole);
}

void RamObjectView::setSortable(bool sortable)
{
    this->verticalHeader()->setSectionsMovable(sortable);
}

void RamObjectView::setSortRole(RamObject::DataRole role)
{
    m_objectModel->setSortRole(role);
    m_objectModel->sort(0);
}

void RamObjectView::sort()
{
    m_objectModel->sort(0);
}

void RamObjectView::setCompletionRatio(bool showCompletion)
{
    m_delegate->setCompletionRatio(showCompletion);
}

void RamObjectView::showDetails(bool s)
{
    m_delegate->showDetails(s);
    this->resizeRowsToContents();
}

void RamObjectView::search(QString s)
{
    // Search
    m_objectModel->search(s);
    this->resizeRowsToContents();
}

void RamObjectView::mousePressEvent(QMouseEvent *event)
{
    // Simple click detection
    if (event->button() == Qt::LeftButton && event->modifiers().testFlag(Qt::NoModifier))
    {
        // Get the index
        m_clicking = this->indexAt( event->localPos().toPoint() );
    }
    DuTableView::mousePressEvent(event);
}

void RamObjectView::mouseReleaseEvent(QMouseEvent *event)
{
    // Simple click to select
    if (m_clicking.isValid())
    {
        // Check index
        QModelIndex testIndex = this->indexAt( event->localPos().toPoint() );
        if (testIndex == m_clicking && event->modifiers().testFlag(Qt::NoModifier)) select(m_clicking);
        // Release
        m_clicking = QModelIndex();
    }
    DuTableView::mouseReleaseEvent(event);
}

void RamObjectView::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    if (m_displayMode == List) {
        this->horizontalHeader()->setStretchLastSection(false);
        this->horizontalHeader()->setStretchLastSection(true);
    }
    else {
        // Incorrect draw after resize, fixed by resizing the first row...
        int rowHeight = this->rowHeight(0);
        this->setRowHeight(0,5);
        this->setRowHeight(0,rowHeight);
    }

    DuTableView::resizeEvent(event);
}

void RamObjectView::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)

    if (m_layout) return;
    //this->resizeRowsToContents();
    //this->resizeColumnsToContents();
    m_layout = true;

    DuTableView::showEvent(event);
}

void RamObjectView::select(const QModelIndex &index)
{
    RamObject *obj = m_objectModel->get(index);
    if (!obj) return;
    this->selectionModel()->select(index, QItemSelectionModel::ClearAndSelect);
    emit objectSelected(obj);
}

void RamObjectView::contextMenuRequested(QPoint p)
{
    if (m_contextMenuDisabled) return;

    // Check the filename
    QModelIndex index = this->currentIndex();
    QString f = index.data(RamObject::FileName).toString();
    if (f == "") ui_actionCopyFileName->setVisible(false);
    else ui_actionCopyFileName->setVisible(true);

    ui_contextMenu->popup(this->viewport()->mapToGlobal(p));
}

void RamObjectView::copyUuid()
{
    QModelIndex index = this->currentIndex();
    QString uuid = index.data(RamObject::UUID).toString();
    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setText( uuid );
}

void RamObjectView::copyPath()
{
    QModelIndex index = this->currentIndex();
    QString path = index.data(RamObject::Path).toString();

    const QFileInfo fileInfo(path);
    if(!fileInfo.exists())
    {
        QMessageBox::StandardButton rep = QMessageBox::question(nullptr,
                                                                "The folder does not exist",
                                                                "This folder:\n\n" + path + "\n\ndoes not exist yet.\nDo you want to create it now?",
                                                                QMessageBox::Yes | QMessageBox::No,
                                                                QMessageBox::Yes);
        if (rep == QMessageBox::Yes) QDir(path).mkpath(".");
    }

    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setText( path );
}

void RamObjectView::copyFileName()
{
    QModelIndex index = this->currentIndex();
    QString f = index.data(RamObject::FileName).toString();
    if (f == "") return;
    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setText( f );
}

void RamObjectView::rowMoved(int logicalIndex, int oldVisualIndex, int newVisualIndex)
{
    Q_UNUSED(logicalIndex);

    QSignalBlocker b(this->verticalHeader());

    //m_objectList->moveRow(QModelIndex(), oldVisualIndex, QModelIndex(), newVisualIndex);

    // Get the source model to move the row
    QAbstractItemModel *model = m_objectModel->sourceModel();
    model->moveRow(QModelIndex(), oldVisualIndex, QModelIndex(), newVisualIndex);
    // move back to the (new) logical index
    this->verticalHeader()->moveSection(newVisualIndex, oldVisualIndex);

    // Convert the filtered index to the model index
    /*QModelIndex oldIndex = m_objectModel->index(oldVisualIndex, 0);
    QModelIndex newIndex = m_objectModel->index(newVisualIndex, 0);

    oldIndex = m_objectModel->mapToSource(oldIndex);
    newIndex = m_objectModel->mapToSource(newIndex);

    model->moveRow(QModelIndex(), oldIndex.row(), QModelIndex(), newIndex.row());

    // move back to the (new) logical index
    this->verticalHeader()->moveSection(newVisualIndex, oldVisualIndex);//*/
}

void RamObjectView::select(RamObject *o)
{
    if (!m_objectModel) return;
    for (int i = 0; i< m_objectModel->rowCount(); i++)
    {
        RamObject *obj = m_objectModel->get(i);
        if (obj->is(o))
        {
            QModelIndex index = m_objectModel->index(i, 0);
            this->setCurrentIndex( index );
            this->select( index );
            this->scrollTo(index, QAbstractItemView::PositionAtTop);
            RamShot *shot = RamShot::c(o);
            if (shot) TimelineManager::instance()->setCurrentShot(shot);
            return;
        }
    }
}

void RamObjectView::filter(RamObject *o)
{
    if (!o) m_objectModel->setFilterUuid("");
    else m_objectModel->setFilterUuid(o->uuid());
    this->resizeRowsToContents();
}

void RamObjectView::setupUi()
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
    this->setColumnWidth( 0, this->size().width() );
    this->setMouseTracking(true);
    this->setContextMenuPolicy(Qt::CustomContextMenu);

    m_delegate = new RamObjectDelegate();
    this->setItemDelegate( m_delegate );

    // Context menu
    ui_contextMenu = new DuMenu(this);

    ui_actionCopyFileName = new QAction(tr("Copy file name"));
    ui_actionCopyFileName->setIcon(DuIcon(":/icons/filename"));
    ui_contextMenu->addAction(ui_actionCopyFileName);

    ui_actionCopyPath = new QAction(tr("Copy folder path"));
    ui_actionCopyPath->setIcon(DuIcon(":/icons/path"));
    ui_contextMenu->addAction(ui_actionCopyPath);

    ui_actionCopyUuid = new QAction(tr("Copy UUID"));
    ui_actionCopyUuid->setIcon(DuIcon(":/icons/code"));
    ui_contextMenu->addAction(ui_actionCopyUuid);

    // Set model
    m_objectModel = new RamObjectSortFilterProxyModel(this);
    this->setModel(m_objectModel);

    if (m_displayMode == List)
    {
        this->horizontalHeader()->hide();
        this->horizontalHeader()->setStretchLastSection(true);
        m_objectModel->setSingleColumn(true);
    }
}

void RamObjectView::connectEvents()
{
    connect(m_delegate, SIGNAL(edited(RamObject*)), this, SLOT(select(RamObject*)));
    // SORT
    connect( this->verticalHeader(), SIGNAL(sectionMoved(int,int,int)), this, SLOT(rowMoved(int,int,int)));
    // SELECT
    // Unselect before filtering
    connect(m_objectModel, SIGNAL(aboutToFilter()), this->selectionModel(), SLOT(clear()));
    // Context menu
    connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenuRequested(QPoint)));
    connect(ui_actionCopyUuid, SIGNAL(triggered()), this, SLOT( copyUuid() ) );
    connect(ui_actionCopyPath, SIGNAL(triggered()), this, SLOT( copyPath() ) );
    connect(ui_actionCopyFileName, SIGNAL(triggered()), this, SLOT( copyFileName() ) );
}
