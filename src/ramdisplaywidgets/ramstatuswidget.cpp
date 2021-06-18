#include "ramstatuswidget.h"

#include "mainwindow.h"

RamStatusWidget::RamStatusWidget(RamStatus *status, QWidget *parent) :
    RamObjectWidget(status, parent)
{
    _status = status;

    statusEditWidget = new StatusEditWidget(_status, this);
    setEditWidget(statusEditWidget);
    this->dockEditWidget()->setIcon(":/icons/state");

    completeUi();
    objectChanged();

    setUserEditRole(RamUser::Lead);

    connectEvents();//*/
}

RamStatus *RamStatusWidget::status() const
{
    return _status;
}

void RamStatusWidget::showHistoryButton()
{
    m_historyButton->show();
}

void RamStatusWidget::setAdditiveMode()
{
    m_additiveMode = true;
    setUserEditRole(RamUser::Standard);
    setEditable(true);
}

void RamStatusWidget::exploreClicked()
{
    explore(Ramses::instance()->path(_status));
}

void RamStatusWidget::objectChanged()
{
    RamObjectWidget::objectChanged();

    QString title = "";
    if (_status->item()) title += _status->item()->shortName();
    if (_status->step()) title += " | " + _status->step()->shortName();
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
    m_historyButton = new QToolButton();
    m_historyButton->setIcon(QIcon(":/icons/list"));
    m_historyButton->setIconSize(QSize(10,10));
    m_historyButton->hide();
    buttonsLayout->insertWidget(0, m_historyButton);

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
    showExploreButton(true);
}

void RamStatusWidget::connectEvents()
{
    connect(statusEditWidget, &StatusEditWidget::statusUpdated, this, &RamStatusWidget::updateStatus);
    connect(m_historyButton, SIGNAL(clicked()), this, SLOT(showHistory()));
}

void RamStatusWidget::updateStatus(RamState *state, int completion, int version, QString comment)
{
    RamUser *user = _status->user();
    RamUser *currentUser = Ramses::instance()->currentUser();
    if (!user || !currentUser) return;
    if (m_additiveMode && !user->is(currentUser)) // add new
    {
        RamStep *step = _status->step();
        RamItem *item = _status->item();
        if (!step || !item) return;
        item->setStatus(currentUser, state, step, completion, comment, version);
    }
    else // edit current
    {
        _status->setState(state);
        _status->setCompletionRatio(completion);
        _status->setVersion(version);
        _status->setComment(comment);
        _status->update();
    }

}

void RamStatusWidget::showHistory()
{
    if (!m_historyBuilt)
    {
        // Get history
        RamStep *step = _status->step();
        RamItem *item = _status->item();
        if (step && item)
        {
            RamStepStatusHistory *history = item->statusHistory(step);
            m_historyWidget = new StepStatusHistoryWidget(history);

            m_historyDockWidget = new ObjectDockWidget(history);
            m_historyDockWidget->hide();

            QFrame *f = new QFrame(m_historyDockWidget);
            QVBoxLayout *l = new QVBoxLayout();
            l->setContentsMargins(3,3,3,3);
            l->addWidget( m_historyWidget );
            f->setLayout(l);
            m_historyDockWidget->setWidget(f);

            MainWindow *mw = (MainWindow*)GuiUtils::appMainWindow();
            mw->addObjectDockWidget(m_historyDockWidget);

            m_historyBuilt = true;
        }
    }
    if(!m_historyBuilt) return;

    m_historyDockWidget->show();
}

