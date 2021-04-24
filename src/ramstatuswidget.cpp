#include "ramstatuswidget.h"

#include "mainwindow.h"

RamStatusWidget::RamStatusWidget(RamStatus *status, QWidget *parent) :
    RamObjectWidget(status, parent)
{
    _status = status;
    completeUi();
    statusChanged(_status);

    connect(status, &RamObject::changed, this, &RamStatusWidget::statusChanged);
    connect(status, &RamObject::removed, this, &RamStatusWidget::remove);
}

RamStatus *RamStatusWidget::status() const
{
    return _status;
}

void RamStatusWidget::remove()
{
    if (_removing) return;
    this->deleteLater();
}

void RamStatusWidget::statusChanged(RamObject *o)
{
    Q_UNUSED(o);

    QString title = "";
    if (_status->step()) title += _status->step()->name();
    if (_status->state()) title += " | " + _status->state()->shortName();
    title += " (v" + QString::number( _status->version() ) + ")";
    this->setTitle( title );

    QString style = "QProgressBar { background-color:" + _status->state()->color().darker(200).name() + "; } ";
    style += "QProgressBar::chunk {  background-color: " + _status->state()->color().name() + "; }";
    completionBox->setStyleSheet(style);

    completionBox->setValue(_status->completionRatio());
}

void RamStatusWidget::completeUi()
{
    completionBox = new DuQFSlider(this);
    completionBox->setMaximum(100);
    completionBox->setMinimum(0);
    completionBox->showValue(false);
    completionBox->setEditable(false);

    layout->addWidget(completionBox);

    commentEdit = new QPlainTextEdit(this);
    commentEdit->setReadOnly(true);
    commentEdit->setMaximumHeight(100);

    layout->addWidget(commentEdit);
    commentEdit->setPlainText( _status->comment() );
    //adjust edit size
    //QSize textSize = commentEdit->document()->size().toSize();
    //int h = textSize.height() + 3;
    //if ( h < commentEdit->size().height() ) commentEdit->setFixedHeight( h );
}
