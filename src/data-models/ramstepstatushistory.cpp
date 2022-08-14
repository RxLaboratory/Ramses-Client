#include "ramstepstatushistory.h"

#include "ramitem.h"
#include "ramstep.h"
#include "ramstatus.h"
#include "statushistorywidget.h"
#include "mainwindow.h"

RamStepStatusHistory *RamStepStatusHistory::getObject(QString uuid, bool constructNew)
{
    RamObjectList *obj = RamObjectList::getObject(uuid);
    if (!obj && constructNew) return new RamStepStatusHistory( uuid );
    return qobject_cast<RamStepStatusHistory*>( obj );
}

RamStepStatusHistory::RamStepStatusHistory(RamStep *step, RamItem *item):
    RamObjectList(step->shortName(), step->name(), item)
{
    construct();

    m_item = item;
    m_step = step;

    QJsonObject d = RamAbstractObject::data();
    d.insert("item", m_item->uuid());
    d.insert("step", m_step->uuid());
    RamAbstractObject::setData(d);

    connectEvents();
}

RamItem *RamStepStatusHistory::item() const
{
    return m_item;
}

RamStep *RamStepStatusHistory::step() const
{
    return m_step;
}

bool statusSorter(RamObject *a, RamObject *b)
{
    RamStatus *as = qobject_cast<RamStatus*>(a);
    RamStatus *bs = qobject_cast<RamStatus*>(b);
    if (as->date() != bs->date()) return as->date() < bs->date();
    else return a->shortName() < b->shortName();
}

void RamStepStatusHistory::sort(int column, Qt::SortOrder order)
{
    Q_UNUSED(column)
    Q_UNUSED(order)

    std::sort(m_objectList.begin(), m_objectList.end(), statusSorter);
}

void RamStepStatusHistory::edit(bool show)
{
    Q_UNUSED(show)
    if (!ui_editWidget)
    {
        ui_editWidget = new QFrame();
        ui_editWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        QVBoxLayout *layout = new QVBoxLayout(ui_editWidget);
        layout->setContentsMargins(3,3,3,3);
        layout->addWidget( new StatusHistoryWidget( this ) );

    }
    MainWindow *mw = (MainWindow*)GuiUtils::appMainWindow();
    mw->setPropertiesDockWidget(ui_editWidget, m_step->name() + " history", ":/icons/step");
}

// PROTECTED //

RamStepStatusHistory::RamStepStatusHistory(QString uuid, QObject *parent):
    RamObjectList(uuid, parent)
{
    construct();

    QJsonObject d = RamAbstractObject::data();

    m_item = RamItem::getObject(d.value("item").toString(), true);
    m_step = RamStep::getObject(d.value("step").toString(), true);

    connectEvents();
}

void RamStepStatusHistory::rowsChanged(QModelIndex parent, int start, int end)
{
    Q_UNUSED(parent)
    Q_UNUSED(start)

    if (end == rowCount() - 1)
    {
        m_step->computeEstimation();
        emit latestStatusChanged(this);
    }
}

void RamStepStatusHistory::changeData(QModelIndex topLeft, QModelIndex bottomRight, QVector<int> roles)
{
    Q_UNUSED(topLeft)
    Q_UNUSED(roles)

    if (bottomRight.row() == rowCount() - 1)
    {
        m_step->computeEstimation();
        emit latestStatusChanged(this);
    }
}

void RamStepStatusHistory::construct()
{
    m_objectType = StepStatusHistory;
}

void RamStepStatusHistory::connectEvents()
{
    connect(this, SIGNAL(rowsAboutRemoved(QModelIndex,int,int)), this, SLOT(rowsChanged(QModelIndex,int,int)));
    connect(this, SIGNAL(rowsAboutRemoved(QModelIndex,int,int)), this, SLOT(rowsChanged(QModelIndex,int,int)));
    connect(this, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)), this, SLOT(changeData(QModelIndex,QModelIndex,QVector<int>)));
}
