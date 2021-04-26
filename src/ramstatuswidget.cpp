#include "ramstatuswidget.h"

#include "mainwindow.h"

RamStatusWidget::RamStatusWidget(RamStatus *status, QWidget *parent) :
    RamObjectWidget(status, parent)
{
    _status = status;
    completeUi();
    statusChanged(_status);

    connectEvents();
}

RamStatus *RamStatusWidget::status() const
{
    return _status;
}

void RamStatusWidget::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    adjustCommentEditSize();
}

void RamStatusWidget::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);
    adjustCommentEditSize();
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

    // Set completion color
    QString style = "QProgressBar { background-color:" + _status->state()->color().darker(200).name() + "; } ";
    style += "QProgressBar::chunk {  background-color: " + _status->state()->color().name() + "; }";
    completionBox->setStyleSheet(style);

    completionBox->setValue(_status->completionRatio());

    bool hasComment = _status->comment() != "";

    QString userText = _status->date().toString( QSettings().value("dateFormat", "yyyy-MM-dd hh:mm:ss").toString() );

    if (_status->user())
    {
        userText += " | " + _status->user()->name();
        userLabel->setText(userText);
    }

    if (!hasComment) commentEdit->hide();
    commentEdit->setPlainText( _status->comment() );
}

void RamStatusWidget::completeUi()
{
    completionBox = new DuQFSlider(this);
    completionBox->setMaximum(100);
    completionBox->setMinimum(0);
    completionBox->showValue(false);
    completionBox->setEditable(false);
    completionBox->setMaximumHeight(10);

    layout->addWidget(completionBox);

    userLabel = new QLabel(this);
    QString userStyle = "color: " + DuUI::getColor("medium-grey").name() + ";";
    userStyle += "font-style: italic; font-weight: 300;";
    userStyle += "padding-right:" + QString::number(DuUI::getSize("padding","medium")) + "px;";
    userLabel->setStyleSheet(userStyle);
    userLabel->setAlignment(Qt::AlignRight);
    layout->addWidget(userLabel);

    commentEdit = new QPlainTextEdit(this);
    commentEdit->setReadOnly(true);
    commentEdit->setMaximumHeight(100);
    QString style = "background-color: rgba(0,0,0,0);";
    commentEdit->setStyleSheet(style);

    layout->addWidget(commentEdit);

    layout->addStretch();
}

void RamStatusWidget::connectEvents()
{
    connect(_status, &RamObject::changed, this, &RamStatusWidget::statusChanged);
    connect(_status, &RamObject::removed, this, &RamStatusWidget::remove);
}

void RamStatusWidget::adjustCommentEditSize()
{
    // Get text height (returns the number of lines and not the actual height in pixels
    int docHeight = commentEdit->document()->size().toSize().height();
    // Compute needed height in pixels
    int h = docHeight * ( commentEdit->fontMetrics().height() ) + commentEdit->fontMetrics().height() * 2;
    commentEdit->setMaximumHeight(h);
}
