#include "ramstepstatushistory.h"

#include "ramitem.h"
#include "statushistorywidget.h"
#include "mainwindow.h"

RamStepStatusHistory::RamStepStatusHistory(RamStep *step, RamItem *item):
    RamObjectList(step->shortName(), step->name(), item)
{
    m_item = item;
    m_step = step;

    this->setObjectName( "RamStepStatusHistory " + step->shortName() );
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
    if (a->order() != b->order()) return a->order() < b->order();
    else return a->shortName() < b->shortName();
}

void RamStepStatusHistory::sort(int column, Qt::SortOrder order)
{
    Q_UNUSED(column)
    Q_UNUSED(order)

    if (m_sorted) return;
    std::sort(m_objectsList.begin(), m_objectsList.end(), statusSorter);
    m_sorted = false;
}

void RamStepStatusHistory::edit(bool show)
{
    if (!m_editReady)
    {
        StatusHistoryWidget *w = new StatusHistoryWidget(this);
        m_dockWidget = new ObjectDockWidget(m_item);
        QFrame *f = new QFrame(m_dockWidget);
        QVBoxLayout *l = new QVBoxLayout();
        l->setContentsMargins(3,3,3,3);
        l->addWidget(w);
        f->setLayout(l);
        m_dockWidget->setWidget(f);

        MainWindow *mw = (MainWindow*)GuiUtils::appMainWindow();
        mw->addObjectDockWidget(m_dockWidget);
        m_editReady = true;
    }
    m_dockWidget->setVisible(show);
}
