﻿#include "statushistorywidget.h"

StatusHistoryWidget::StatusHistoryWidget(RamStepStatusHistory *history, QWidget *parent) : QWidget(parent)
{
    setupUi();
    ui_statusList->setList(history);
    connectEvents();
}

void StatusHistoryWidget::editObject(RamObject *obj) const
{
    if (obj) obj->edit();
}

void StatusHistoryWidget::setupUi()
{
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(3);

    ui_statusList = new RamObjectListView( RamObjectListView::List, this);
    ui_statusList->setEditableObjects(true, RamUser::Admin);
    layout->addWidget( ui_statusList );
}

void StatusHistoryWidget::connectEvents()
{
    connect(ui_statusList, &RamObjectListView::editObject, this, &StatusHistoryWidget::editObject);
}
