#ifndef SHOTLISTMANAGERWIDGET_H
#define SHOTLISTMANAGERWIDGET_H

#include "objectlistmanagerwidget.h"
#include "shoteditwidget.h"

class ShotListManagerWidget : public ObjectListManagerWidget
{
    Q_OBJECT
public:
    ShotListManagerWidget(QWidget *parent = nullptr);

protected slots:
    void createObject() Q_DECL_OVERRIDE;

private slots:
    void changeProject(RamProject *project);

private:

};

#endif // SHOTSMANAGERWIDGET_H
