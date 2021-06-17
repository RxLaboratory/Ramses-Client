#include "ramstatuswidget.h"

RamStatusWidget::RamStatusWidget(RamStatus *status, QWidget *parent) :
    RamObjectWidget(status, parent)
{
    _status = status;

    statusEditWidget = new StatusEditWidget(_status, this);
    setEditWidget(statusEditWidget);
    this->dockEditWidget()->setIcon(":/icons/state");

    completeUi();
    statusChanged(_status);

    setUserEditRole(RamUser::Lead);

    connectEvents();
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
    this->dockEditWidget()->setTitle( title );

    title += " (v" + QString::number( _status->version() ) + ")";
    this->setTitle( title );

    // Set completion color
    QString style = "QProgressBar { background-color:" + _status->state()->color().darker(200).name() + "; } ";
    style += "QProgressBar::chunk {  background-color: " + _status->state()->color().name() + "; }";
    completionBox->setStyleSheet(style);

    completionBox->setValue(_status->completionRatio());

    QString userText = _status->date().toString( QSettings().value("dateFormat", "yyyy-MM-dd hh:mm:ss").toString() );

    if (_status->user())
    {
        userText += " | " + _status->user()->name();
        userLabel->setText(userText);
    }
}

void RamStatusWidget::completeUi()
{
    completionBox = new DuQFSlider(this);
    completionBox->setMaximum(100);
    completionBox->setMinimum(0);
    completionBox->showValue(false);
    completionBox->setEditable(false);
    completionBox->setMaximumHeight(10);

    primaryContentLayout->addWidget(completionBox);

    userLabel = new QLabel(this);
    QString userStyle = "color: " + DuUI::getColor("medium-grey").name() + ";";
    userStyle += "font-style: italic; font-weight: 300;";
    userStyle += "padding-right:" + QString::number(DuUI::getSize("padding","medium")) + "px;";
    userLabel->setStyleSheet(userStyle);
    userLabel->setAlignment(Qt::AlignRight);
    primaryContentLayout->addWidget(userLabel);

    setPrimaryContentHeight(40);
    primaryContentWidget->show();
    setAlwaysShowPrimaryContent(true);
}

void RamStatusWidget::connectEvents()
{
    connect(_status, &RamObject::changed, this, &RamStatusWidget::statusChanged);
    connect(_status, &RamObject::removed, this, &RamStatusWidget::remove);
    connect(statusEditWidget, &StatusEditWidget::statusUpdated, this, &RamStatusWidget::updateStatus);
}

void RamStatusWidget::updateStatus(RamState *state, int completion, int version, QString comment)
{
    _status->setState(state);
    _status->setCompletionRatio(completion);
    _status->setVersion(version);
    _status->setComment(comment);
    _status->update();
}
