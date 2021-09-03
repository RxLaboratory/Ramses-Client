#ifndef SHOTLISTMANAGERWIDGET_H
#define SHOTLISTMANAGERWIDGET_H

#include "objectlistmanagerwidget.h"
#include "shoteditwidget.h"
#include "shotscreationdialog.h"

class ShotListManagerWidget : public ObjectListManagerWidget
{
    Q_OBJECT
public:
    ShotListManagerWidget(QWidget *parent = nullptr);

protected slots:
    RamObject *createObject() override;

private slots:
    void changeProject(RamProject *project);
    void batchCreate();

};

#endif // SHOTSMANAGERWIDGET_H
