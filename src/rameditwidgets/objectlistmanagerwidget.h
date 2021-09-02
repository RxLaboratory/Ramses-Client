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
    explicit ObjectListManagerWidget(ObjectEditWidget *editWidget, QString title, QIcon icon = QIcon(), QWidget *parent = nullptr);
    explicit ObjectListManagerWidget(RamObjectList *objectList, ObjectEditWidget *editWidget, QString title, QIcon icon = QIcon(), QWidget *parent = nullptr);
    void setList(RamObjectList *objectList);
    void clear();
    QString currentFilter() const;
    void editObject(RamObject *o);

    QToolButton *menuButton();

protected:
    void showEvent(QShowEvent *event) override;
    ObjectListEditWidget *m_listEditWidget;

protected slots:
    virtual RamObject *createObject() { return nullptr; };
    void createEditObject();;

private:
    void setupUi(QString title, QIcon icon);
    void connectEvents();

    QSplitter *m_splitter;

    ObjectEditWidget *m_editWidget;
    QList<QMetaObject::Connection> m_listConnection;

    QToolButton *ui_itemButton;
    QAction *ui_createAction;
};

#endif // OBJECTLISTMANAGERWIDGET_H
