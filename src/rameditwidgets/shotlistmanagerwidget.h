#ifndef SHOTLISTMANAGERWIDGET_H
#define SHOTLISTMANAGERWIDGET_H

#include "objectlistmanagerwidget.h"
#include "shoteditwidget.h"

class ShotLIstManagerWidget : public ObjectListManagerWidget
{
    Q_OBJECT
public:
    ShotLIstManagerWidget(QWidget *parent = nullptr);

protected slots:
    void createObject() Q_DECL_OVERRIDE;

private slots:
    void changeProject(RamProject *project);
    //void filter(QString sequenceUuid);

private:
    QList<QMetaObject::Connection> _projectConnections;
    QList<QMetaObject::Connection> _sequencesConnections;
};

#endif // SHOTSMANAGERWIDGET_H
