#ifndef STEPSTATUSHISTORYWIDGET_H
#define STEPSTATUSHISTORYWIDGET_H

#include <QWidget>
#include "objectlisteditwidget.h"
#include "data-models/ramstepstatushistory.h"
#include "ramitem.h"

class StepStatusHistoryWidget : public QWidget
{
    Q_OBJECT
public:
    explicit StepStatusHistoryWidget(QWidget *parent = nullptr);
    explicit StepStatusHistoryWidget(RamStepStatusHistory *history, QWidget *parent = nullptr);
    void setHistory(RamStepStatusHistory *history);

signals:

private:
    void setupUi();
    void connectEvents();

    ObjectListEditWidget *m_statusList;

    RamStepStatusHistory *m_history = nullptr;
};

#endif // STEPSTATUSHISTORYWIDGET_H
