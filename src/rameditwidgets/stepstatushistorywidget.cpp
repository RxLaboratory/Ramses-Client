#include "stepstatushistorywidget.h"

StepStatusHistoryWidget::StepStatusHistoryWidget(QWidget *parent) : QWidget(parent)
{
    setupUi();
    setHistory(nullptr);
    connectEvents();
}

StepStatusHistoryWidget::StepStatusHistoryWidget(RamStepStatusHistory *history, QWidget *parent) : QWidget(parent)
{
    setupUi();
    setHistory(history);
    connectEvents();
}

void StepStatusHistoryWidget::setHistory(RamStepStatusHistory *history)
{
    this->setEnabled(false);

    m_history = history;

    if (!m_history) return;

    m_statusList->setList(m_history);
    RamItem *item = m_history->item();
    if (item)
        m_statusList->setTitle("Status history for " + item->name());

    this->setEnabled(true);
}

void StepStatusHistoryWidget::setupUi()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(3);

    m_statusList = new ObjectListEditWidget(true, RamUser::ProjectAdmin, this);
    m_statusList->setTitle("Status history");
    m_statusList->setEditable(false);
    m_statusList->setSearchable(false);
    layout->addWidget( m_statusList );
}

void StepStatusHistoryWidget::connectEvents()
{

}
