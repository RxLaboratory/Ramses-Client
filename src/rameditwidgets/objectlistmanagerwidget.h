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
    void setList(RamObjectList *objectList);
    void clear();
    QString currentFilter() const;
    void editObject(RamObject *o);

protected:
    void showEvent(QShowEvent *event) override;
    ObjectListEditWidget *m_listEditWidget;

protected slots:
    virtual void createObject() {};

private:
    void setupUi();
    void connectEvents();

    QSplitter *m_splitter;

    ObjectEditWidget *m_editWidget;
    QList<QMetaObject::Connection> m_listConnection;
};

#endif // OBJECTLISTMANAGERWIDGET_H
