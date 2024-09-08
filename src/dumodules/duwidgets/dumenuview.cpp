#include "dumenuview.h"

DuMenuView::DuMenuView(const QString &title, QWidget *parent):
    DuMenu(title, parent)
{
    connect(this, &DuMenu::triggered, this, [this] (QAction *a) {
        if (!a || !_model)
            return;
        int i = a->data().toInt();
        if (i < 0)
            return;
        if (i >= _model->rowCount())
            return;
        emit indexTriggered( _model->index(i, 0) );
    });
}

QAbstractItemModel *DuMenuView::model() const
{
    return _model;
}

void DuMenuView::setModel(QAbstractItemModel *newModel)
{
    if (_model)
        disconnect(_model, nullptr, this, nullptr);

    _model = newModel;

    connect(_model, &QAbstractItemModel::modelAboutToBeReset, this, &DuMenuView::clear);
    connect(_model, &QAbstractItemModel::modelReset, this, &DuMenuView::rebuild);

    connect(_model, &QAbstractItemModel::rowsAboutToBeInserted, this, &DuMenuView::clear);
    connect(_model, &QAbstractItemModel::rowsInserted, this, &DuMenuView::rebuild);

    connect(_model, &QAbstractItemModel::rowsAboutToBeMoved, this, &DuMenuView::clear);
    connect(_model, &QAbstractItemModel::rowsMoved, this, &DuMenuView::rebuild);

    connect(_model, &QAbstractItemModel::rowsAboutToBeRemoved, this, &DuMenuView::clear);
    connect(_model, &QAbstractItemModel::rowsRemoved, this, &DuMenuView::rebuild);

    connect(_model, &QAbstractItemModel::dataChanged, this, &DuMenuView::updateData);

    clear();
    rebuild();
}

void DuMenuView::clear()
{
    qDeleteAll(_actions);
    _actions.clear();
}

void DuMenuView::rebuild()
{
    for (int i = 0; i < _model->rowCount(); ++i) {

        QModelIndex index = _model->index(i, 0);

        QAction *a = new QAction(this);
        a->setIcon( index.data(Qt::DecorationRole).value<QIcon>() );
        a->setText( index.data(Qt::DisplayRole).toString() );
        a->setToolTip( index.data(Qt::ToolTipRole).toString() );
        a->setStatusTip( index.data(Qt::StatusTipRole).toString() );
        a->setData(i);

        this->addAction(a);
        _actions << a;
    }
}

void DuMenuView::updateData(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
    int first = topLeft.row();
    int last = bottomRight.row();
    for (int i = first; i <= last; ++i) {
        QAction *a = _actions.at(i);
        QModelIndex index = _model->index(i, 0);
        for (int r: roles) {
            switch(r) {
            case Qt::DecorationRole:
                a->setIcon( index.data(Qt::DecorationRole).value<QIcon>() );
                break;
            case Qt::DisplayRole:
                a->setText( index.data(Qt::DisplayRole).toString() );
                break;
            case Qt::ToolTipRole:
                a->setToolTip( index.data(Qt::ToolTipRole).toString() );
                break;
            case Qt::StatusTipRole:
                a->setStatusTip( index.data(Qt::StatusTipRole).toString() );
                break;
            }
        }
    }
}
