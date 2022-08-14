#ifndef SHOTLISTMANAGERWIDGET_H
#define SHOTLISTMANAGERWIDGET_H

#include "objectlistmanagerwidget.h"

class ShotListManagerWidget : public ObjectListManagerWidget
{
    Q_OBJECT
public:
    ShotListManagerWidget(QWidget *parent = nullptr);

protected slots:
    RamShot *createObject() override;

private slots:
    void changeProject(RamProject *project);
    void batchCreate();

};

#endif // SHOTSMANAGERWIDGET_H
