#ifndef STEPSTATUSHISTORYWIDGET_H
#define STEPSTATUSHISTORYWIDGET_H

#include <QWidget>
#include <QClipboard>

#include "objectlisteditwidget.h"
#include "data-models/ramstepstatushistory.h"

class StepStatusHistoryWidget : public QWidget
{
public:
    explicit StepStatusHistoryWidget(QWidget *parent = nullptr);
    explicit StepStatusHistoryWidget(RamStepStatusHistory *history, QWidget *parent = nullptr);
    void setHistory(RamStepStatusHistory *history);

private:
    void setupUi();
    void connectEvents();

    ObjectListEditWidget<RamStatus*, RamStep*> *m_statusList;

    RamStepStatusHistory *m_history = nullptr;
};

#endif // STEPSTATUSHISTORYWIDGET_H
