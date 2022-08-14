#ifndef SEQUENCELISTMANAGERWIDGET_H
#define SEQUENCELISTMANAGERWIDGET_H

#include "objectlistmanagerwidget.h"
#include "ramsequence.h"

class SequenceListManagerWidget : public ObjectListManagerWidget
{
    Q_OBJECT
public:
    SequenceListManagerWidget(QWidget *parent = nullptr);

protected slots:
    RamSequence *createObject() override;

private slots:
    void changeProject(RamProject *project);

private:

};

#endif // SEQUENCELISTMANAGERWIDGET_H
