#ifndef RAMSEQUENCEWIDGET_H
#define RAMSEQUENCEWIDGET_H

#include "ramobjectwidget.h"
#include "sequenceeditwidget.h"

class RamSequenceWidget : public RamObjectWidget
{
    Q_OBJECT
public:
    RamSequenceWidget(RamSequence *sequence, QWidget *parent = nullptr);
    RamSequence *sequence() const;
private:
    RamSequence *m_sequence;
};

#endif // RAMSEQUENCEWIDGET_H
