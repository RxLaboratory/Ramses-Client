#ifndef SEQUENCEEDITWIDGET_H
#define SEQUENCEEDITWIDGET_H

#include "ui_sequenceeditwidget.h"
#include "ramses.h"

class SequenceEditWidget : public QWidget, private Ui::SequenceEditWidget
{
    Q_OBJECT

public:
    explicit SequenceEditWidget(QWidget *parent = nullptr);

    RamSequence *sequence() const;
    void setSequence(RamSequence *sequence);

private slots:
    void update();
    void revert();
    bool checkInput();
    void sequenceRemoved(RamObject *o);
    void dbiLog(DuQFLog m);

private:
    RamSequence *_sequence;
    QList<QMetaObject::Connection> _sequenceConnections;
};

#endif // SEQUENCEEDITWIDGET_H
