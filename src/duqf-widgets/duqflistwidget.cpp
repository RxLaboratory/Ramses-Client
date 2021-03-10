#include "duqflistwidget.h"

DuQFListWidget::DuQFListWidget(QWidget *parent): QListWidget(parent)
{
    this->setDragDropMode(InternalMove);
    this->setDragEnabled(true);
    this->setDefaultDropAction(Qt::MoveAction);
    this->setSelectionMode(ExtendedSelection);
}

void DuQFListWidget::dropEvent(QDropEvent *event)
{
    QListWidget::dropEvent(event);
    emit itemDropped();
}
