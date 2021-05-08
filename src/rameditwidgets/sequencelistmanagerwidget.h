#ifndef SEQUENCELISTMANAGERWIDGET_H
#define SEQUENCELISTMANAGERWIDGET_H

#include "objectlistmanagerwidget.h"
#include "sequenceeditwidget.h"

class SequenceListManagerWidget : public ObjectListManagerWidget
{
    Q_OBJECT
public:
    SequenceListManagerWidget(QWidget *parent = nullptr);

protected slots:
    void createObject() Q_DECL_OVERRIDE;

private slots:
    void changeProject(RamProject *project);

private:

};

#endif // SEQUENCELISTMANAGERWIDGET_H
