#ifndef OBJECTLISTMANAGERWIDGET_H
#define OBJECTLISTMANAGERWIDGET_H

#include <QWidget>
#include <QSplitter>

#include "objecteditwidget.h"
#include "objectlisteditwidget.h"

class ObjectListManagerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ObjectListManagerWidget(ObjectEditWidget *editWidget, QString title, QWidget *parent = nullptr);
    explicit ObjectListManagerWidget(RamObjectList *objectList, ObjectEditWidget *editWidget, QString title, QWidget *parent = nullptr);
    explicit ObjectListManagerWidget(RamObjectUberList *objectList, ObjectEditWidget *editWidget, QString title, QWidget *parent = nullptr);
    void setContainingType(RamObject::ObjectType type);
    void setSortable(bool sortable);
    void setList(RamObjectList *objectList);
    void setList(RamObjectUberList *objectList);
    void clear();
    QString currentFilter() const;

protected:
    void showEvent(QShowEvent *event) Q_DECL_OVERRIDE;

protected slots:
    virtual void createObject() {};

private slots:
    void editNewObject(RamObject *o);

private:
    void setupUi();
    void connectEvents();

    QSplitter *m_splitter;
    ObjectListEditWidget *m_listEditWidget;
    ObjectEditWidget *m_editWidget;
    QList<QMetaObject::Connection> m_listConnection;
};

#endif // OBJECTLISTMANAGERWIDGET_H
