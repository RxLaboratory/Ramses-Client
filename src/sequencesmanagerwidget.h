#ifndef SEQUENCESMANAGERWIDGET_H
#define SEQUENCESMANAGERWIDGET_H

#include "listmanagerwidget.h"
#include "sequenceeditwidget.h"

class SequencesManagerWidget : public ListManagerWidget
{
    Q_OBJECT
public:
    SequencesManagerWidget(QWidget *parent = nullptr);

protected slots:
    void currentDataChanged(QVariant data) Q_DECL_OVERRIDE;
    void createItem() Q_DECL_OVERRIDE;
    void removeItem(QVariant data) Q_DECL_OVERRIDE;

private slots:
    void changeProject(RamProject *project);
    void newSequence(RamSequence *seq);
    void removeSequence(RamSequence *seq);
    void sequenceChanged(RamSequence *seq);

private:
    SequenceEditWidget *sequenceWidget;
    QList<QMetaObject::Connection> _projectConnections;
};

#endif // SEQUENCESMANAGERWIDGET_H
