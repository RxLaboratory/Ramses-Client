#include "ramsequencewidget.h"

#include "mainwindow.h"

RamSequenceWidget::RamSequenceWidget(RamSequence *sequence, QWidget *parent):
    RamObjectWidget(sequence, parent)
{
    m_sequence = sequence;

    SequenceEditWidget *sw = new SequenceEditWidget(sequence, this);
    setEditWidget(sw);
    MainWindow *mw = (MainWindow*)GuiUtils::appMainWindow();
    mw->addSequenceEditDockWidget(this->dockEditWidget());

    setUserEditRole(RamUser::ProjectAdmin);

    setIcon(":/icons/sequence");

}

RamSequence *RamSequenceWidget::sequence() const
{
    return m_sequence;
}
