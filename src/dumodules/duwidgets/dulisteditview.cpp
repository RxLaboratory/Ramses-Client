#include "dulisteditview.h"

#include <QLabel>

#include "duapp/duui.h"
#include "duicon.h"
#include "dutableview.h"

DuListEditView::DuListEditView(const QString &title, const QString &itemName, QAbstractItemModel *model, QWidget *parent)
    : QWidget{parent},
    _model(model),
    _title(title),
    _itemName(itemName)
{
    setupUi();
    connectEvents();
    updateList();
}

void DuListEditView::setAssignList(QAbstractItemModel *model, bool removeCreateButton)
{
    ui_assignMenu->setModel(model);

    if (model) {

        if (!removeCreateButton) {
            DuMenu *m = new DuMenu(this);
            m->addAction(_addAction);
            m->addMenu(ui_assignMenu);
            ui_addButton->setMenu(m);
        }
        else {
            ui_addButton->setMenu(ui_assignMenu);
        }

    }
    else {
        QMenu *m = ui_addButton->menu();
        if (m != ui_assignMenu) m->deleteLater();
        else ui_addButton->setMenu(nullptr);
    }
}

void DuListEditView::add()
{
    int row = _model->rowCount();
    _model->insertRows(row, 1);
    QModelIndex index = _model->index(row,0);
    // Select it
    QItemSelectionModel *selection = ui_listView->selectionModel();
    selection->select(index, QItemSelectionModel::ClearAndSelect);
    emit editing(index);
}

void DuListEditView::remove()
{
    QItemSelectionModel *selection = ui_listView->selectionModel();
    if (!selection->hasSelection())
        return;

    const QModelIndexList &items = selection->selectedRows();
    for(int i = items.count()-1; i >= 0; --i) {
        _model->removeRows(items.at(i).row(),1);
    }
}

void DuListEditView::moveUp()
{
    QItemSelectionModel *selection = ui_listView->selectionModel();
    if (!selection->hasSelection())
        return;

    const QModelIndexList &items = selection->selectedRows();
    for(int i = 0; i < items.count(); ++i) {
        int r = items.at(i).row();
        if (r ==  0)
            continue;
        _model->moveRows(QModelIndex(),
                        r,
                        1,
                        QModelIndex(),
                        r-1
                        );
    }
}

void DuListEditView::moveDown()
{
    QItemSelectionModel *selection = ui_listView->selectionModel();
    if (!selection->hasSelection())
        return;

    const QModelIndexList &items = selection->selectedRows();
    for(int i = items.count()-1; i >= 0; --i) {
        int r = items.at(i).row();
        if (r == _model->rowCount() -1)
            continue;
        _model->moveRows(QModelIndex(),
                        r,
                        1,
                        QModelIndex(),
                        r+2
                        );
    }
}

void DuListEditView::clear()
{
    _model->removeRows(0, _model->rowCount());
}

void DuListEditView::updateList()
{
    bool canMove = _model->rowCount() > 1;
    ui_moveUpButton->setEnabled(canMove);
    ui_moveDownButton->setEnabled(canMove);

    bool canRemove = _model->rowCount() > 0;
    ui_removeButton->setEnabled(canRemove);
    ui_clearButton->setEnabled(canRemove);
}

void DuListEditView::assign(const QModelIndex &index)
{
    QVariant data = index.data(Qt::EditRole);
    int i = _model->rowCount();
    _model->insertRows(i,1);
    _model->setData(_model->index(i,0), data);
}

void DuListEditView::setupUi()
{
    auto layout = DuUI::addBoxLayout(Qt::Vertical, this);
    auto topLayout = DuUI::addBoxLayout(Qt::Horizontal, layout);

    ui_moveUpButton = new QToolButton(this);
    ui_moveUpButton->setText(tr("Move up"));
    ui_moveUpButton->setIcon(DuIcon(":/icons/move-up"));
    ui_moveUpButton->setIconSize(QSize(12,12));
    topLayout->addWidget(ui_moveUpButton);

    ui_moveDownButton = new QToolButton(this);
    ui_moveDownButton->setText(tr("Move down"));
    ui_moveDownButton->setIcon(DuIcon(":/icons/move-down"));
    ui_moveDownButton->setIconSize(QSize(12,12));
    topLayout->addWidget(ui_moveDownButton);

    auto titleLabel = new QLabel(_title, this);
    titleLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    topLayout->addWidget(titleLabel);

    ui_clearButton = new QToolButton(this);
    ui_clearButton->setText(tr("Remove all"));
    ui_clearButton->setIcon(DuIcon(":/icons/clean"));
    ui_clearButton->setIconSize(QSize(12,12));
    topLayout->addWidget(ui_clearButton);

    ui_removeButton = new QToolButton(this);
    ui_removeButton->setText(tr("Remove"));
    ui_removeButton->setIcon(DuIcon(":/icons/remove"));
    ui_removeButton->setIconSize(QSize(12,12));
    topLayout->addWidget(ui_removeButton);

    ui_addButton = new QToolButton(this);
    ui_addButton->setText(tr("Add"));
    ui_addButton->setIcon(DuIcon(":/icons/add"));
    ui_addButton->setIconSize(QSize(12,12));
    ui_addButton->setPopupMode(QToolButton::InstantPopup);
    topLayout->addWidget(ui_addButton);

    ui_listView = new DuListView(this);
    //ui_listView->horizontalHeader()->hide();
    //ui_listView->setVerticalHeaderMoveData();
    //ui_listView->verticalHeader()->setSectionsMovable(true);
    ui_listView->setModel(_model);
    layout->addWidget(ui_listView);

    _addAction = new QAction(this);
    _addAction->setText(tr("Create new %1").arg(_itemName));

    ui_assignMenu = new DuMenuView(tr("Assign %1").arg(_itemName), this);
}

void DuListEditView::connectEvents()
{
    connect(ui_addButton, &QToolButton::clicked, this, &DuListEditView::add);
    connect(_addAction, &QAction::triggered, this, &DuListEditView::add);
    connect(ui_removeButton, &QToolButton::clicked, this, &DuListEditView::remove);
    connect(ui_moveUpButton, &QToolButton::clicked, this, &DuListEditView::moveUp);
    connect(ui_moveDownButton, &QToolButton::clicked, this, &DuListEditView::moveDown);
    connect(ui_clearButton, &QToolButton::clicked, this, &DuListEditView::clear);
    connect(ui_listView, &DuTableView::clicked, this, &DuListEditView::editing);
    connect(ui_assignMenu, &DuMenuView::indexTriggered, this, &DuListEditView::assign);

    connect(_model, &QAbstractItemModel::rowsInserted, this, &DuListEditView::updateList);
    connect(_model, &QAbstractItemModel::rowsRemoved, this, &DuListEditView::updateList);
}
