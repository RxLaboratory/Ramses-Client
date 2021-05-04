#include "ramsequencewidget.h"

RamSequenceWidget::RamSequenceWidget(RamSequence *sequence, QWidget *parent):
    RamObjectWidget(sequence, parent)
{
    m_sequence = sequence;

    SequenceEditWidget *sw = new SequenceEditWidget(sequence, this);
    setEditWidget(sw);

    setUserEditRole(RamUser::ProjectAdmin);

    setIcon(":/icons/sequence");

}

RamSequence *RamSequenceWidget::sequence() const
{
    return m_sequence;
}
