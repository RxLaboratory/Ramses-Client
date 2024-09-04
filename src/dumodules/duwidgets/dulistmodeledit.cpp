#include "dulistmodeledit.h"

#include <QLabel>

#include "duapp/duui.h"
#include "duicon.h"

DuListModelEdit::DuListModelEdit(const QString &title, QAbstractItemModel *model, QWidget *parent)
    : QWidget{parent},
    _model(model)
{
    setupUi(title);
    connectEvents();
    updateList();
}

void DuListModelEdit::add()
{
    int row = _model->rowCount();
    _model->insertRows(row, 1);
    emit editing(_model->index(row,0));
}

void DuListModelEdit::remove()
{
    QItemSelectionModel *selection = ui_listView->selectionModel();
    if (!selection->hasSelection())
        return;

    const QModelIndexList &items = selection->selectedRows();
    for(int i = items.count()-1; i >= 0; --i) {
        _model->removeRows(items.at(i).row(),1);
    }
}

void DuListModelEdit::moveUp()
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

void DuListModelEdit::moveDown()
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

void DuListModelEdit::clear()
{
    _model->removeRows(0, _model->rowCount());
}

void DuListModelEdit::updateList()
{
    bool canMove = _model->rowCount() > 1;
    ui_moveUpButton->setEnabled(canMove);
    ui_moveDownButton->setEnabled(canMove);

    bool canRemove = _model->rowCount() > 0;
    ui_removeButton->setEnabled(canRemove);
    ui_clearButton->setEnabled(canRemove);
}

void DuListModelEdit::setupUi(const QString &title)
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

    auto titleLabel = new QLabel(title, this);
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
    topLayout->addWidget(ui_addButton);

    ui_listView = new DuListView(this);
    ui_listView->setDragDropMode(QAbstractItemView::NoDragDrop);
    ui_listView->setDragEnabled(false);
    ui_listView->setDefaultDropAction(Qt::IgnoreAction);
    ui_listView->setModel(_model);
    layout->addWidget(ui_listView);

}

void DuListModelEdit::connectEvents()
{
    connect(ui_addButton, &QToolButton::clicked, this, &DuListModelEdit::add);
    connect(ui_removeButton, &QToolButton::clicked, this, &DuListModelEdit::remove);
    connect(ui_moveUpButton, &QToolButton::clicked, this, &DuListModelEdit::moveUp);
    connect(ui_moveDownButton, &QToolButton::clicked, this, &DuListModelEdit::moveDown);
    connect(ui_clearButton, &QToolButton::clicked, this, &DuListModelEdit::clear);

    connect(_model, &QAbstractItemModel::rowsInserted, this, &DuListModelEdit::updateList);
    connect(_model, &QAbstractItemModel::rowsRemoved, this, &DuListModelEdit::updateList);
}
