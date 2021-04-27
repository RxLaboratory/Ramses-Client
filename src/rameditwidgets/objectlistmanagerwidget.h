#ifndef OBJECTLISTMANAGERWIDGET_H
#define OBJECTLISTMANAGERWIDGET_H

#include <QWidget>
#include "objecteditwidget.h"
#include "objectlisteditwidget.h"

class ObjectListManagerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ObjectListManagerWidget(RamObjectList *objectList, ObjectEditWidget *editWidget, QString title, QWidget *parent = nullptr);

    RamObjectList *objectList() const;

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
};

#endif // OBJECTLISTMANAGERWIDGET_H
