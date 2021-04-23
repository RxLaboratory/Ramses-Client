#include "ramstatuswidget.h"

#include "mainwindow.h"

RamStatusWidget::RamStatusWidget(RamStatus *status, QWidget *parent) :
    RamObjectWidget(status, parent)
{
    _status = status;
    completeUi();
    setEdibable(false);

    connect(status, &RamObject::changed, this, &RamStatusWidget::statusChanged);
    connect(status, &RamObject::removed, this, &RamStatusWidget::remove);
}

RamStatus *RamStatusWidget::status() const
{
    return _status;
}

void RamStatusWidget::setEdibable(bool editable)
{
    completionBox->setEnabled(editable);
}

void RamStatusWidget::remove()
{
    if (_removing) return;
    this->deleteLater();
}

void RamStatusWidget::statusChanged(RamObject *o)
{
    Q_UNUSED(o);

    completionBox->setValue(_status->completionRatio());
}

void RamStatusWidget::completeUi()
{
    QString title = "";
    if (_status->step()) title += _status->step()->name();
    if (_status->state()) title += " | " + _status->state()->shortName();
    this->setTitle( title );

    completionBox = new DuQFSpinBox(this);
    completionBox->setMaximum(100);
    completionBox->setMinimum(0);
    completionBox->setSuffix("%");
    completionBox->setValue(_status->completionRatio());

    layout->addWidget(completionBox);
}
