#ifndef OBJECTLISTMANAGERWIDGET_H
#define OBJECTLISTMANAGERWIDGET_H

#include <QWidget>
#include "objecteditwidget.h"
#include "objectlisteditwidget.h"

class ObjectListManagerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ObjectListManagerWidget(ObjectEditWidget *editWidget, QString title, QWidget *parent = nullptr);
    explicit ObjectListManagerWidget(RamObjectList *objectList, ObjectEditWidget *editWidget, QString title, QWidget *parent = nullptr);
    void setList(RamObjectList *objectList);
    RamObjectList *objectList() const;
    QString currentFilter() const;

protected slots:
    virtual void createObject() = 0;

private slots:
    void editNewObject(RamObject *o);

private:
    void setupUi();
    void connectEvents();

    ObjectListEditWidget *m_listEditWidget;
    ObjectEditWidget *m_editWidget;
    RamObjectList *m_objectList;
    QMetaObject::Connection m_listConnection;
};

#endif // OBJECTLISTMANAGERWIDGET_H
