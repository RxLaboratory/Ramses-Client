#ifndef STATUSHISTORYWIDGET_H
#define STATUSHISTORYWIDGET_H

#include <QSplitter>

#include "ramitem.h"
#include "statuseditwidget.h"
#include "ramobjectlistview.h"

class StatusHistoryWidget : public QWidget
{
    Q_OBJECT
public:
    explicit StatusHistoryWidget(RamStepStatusHistory *history, QWidget *parent = nullptr);

signals:

private slots:
    void editObject(RamObject *obj) const;

private:
    void setupUi();
    void connectEvents();
    RamObjectListView *ui_statusList;
};

#endif // STATUSHISTORYWIDGET_H
