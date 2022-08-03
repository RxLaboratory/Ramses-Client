#include "ramstepstatushistory.h"

#include "ramitem.h"
#include "ramstep.h"
#include "ramstatus"
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
