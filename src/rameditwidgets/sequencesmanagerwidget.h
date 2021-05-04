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
    void newSequence(RamObject *seq);
    void removeSequence(RamObject *seq);
    void sequenceChanged(RamObject *seq);

private:
    SequenceEditWidget *sequenceWidget;
    QList<QMetaObject::Connection> _projectConnections;
};

#endif // SEQUENCESMANAGERWIDGET_H
