#ifndef STATUSHISTORYWIDGET_H
#define STATUSHISTORYWIDGET_H

#include <QSplitter>

//#include "duqf-widgets/duqflistwidget.h"
#include "ramitem.h"
#include "ramstatuswidget.h"
#include "statuseditwidget.h"
#include "objectlisteditwidget.h"

class SimpleObjectList;

/**
 * @brief The StatusHistoryWidget class shows the status history of any RamItem
 * It has a RamStep selector to select the step shown,
 * A list of past status
 * And a status editor to add a new status for the selected step
 * TODO: change the combobox for the steps to a new StepSelectorWidget class, working like the ProjectSelectorWidget class
 */
class StatusHistoryWidget : public QWidget
{
    Q_OBJECT
public:
    explicit StatusHistoryWidget(QWidget *parent = nullptr);
    explicit StatusHistoryWidget(RamItem *item, QWidget *parent = nullptr);
    void setItem(RamItem *item);

signals:

private slots:
    void newStep(RamObject *obj);
    void stepRemoved(RamObject *step);

    void currentProjectChanged(RamProject *project);

    void setStatus(RamState *state, int completionRatio, int version, QString comment);

    void currentFilterChanged(QString f);

private:
    RamItem *_item;

    QList<QMetaObject::Connection> _itemConnections;
    QList<QMetaObject::Connection> _projectConnections;

    void setupUi();
    void connectEvents();
    ObjectListEditWidget *statusList;
    StatusEditWidget *statusWidget;
};

#endif // STATUSHISTORYWIDGET_H
