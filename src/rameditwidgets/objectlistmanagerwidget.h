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
    explicit ObjectListManagerWidget(QString title, QIcon icon = QIcon(), QWidget *parent = nullptr);
    explicit ObjectListManagerWidget(RamObjectList *objectList, QString title, QIcon icon = QIcon(), QWidget *parent = nullptr);
    void setList(RamObjectList *objectList);
    void clear();
    QString currentFilter() const;

    QToolButton *menuButton();

protected:
    ObjectListEditWidget *m_listEditWidget;

protected slots:
    virtual RamObject *createObject() { return nullptr; };
    void createEditObject();

private:
    void setupUi(QString title, QIcon icon);
    void connectEvents();

    QList<QMetaObject::Connection> m_listConnection;

    QToolButton *ui_itemButton;
    QAction *ui_createAction;

    QShortcut *ui_createShortcut;
    QShortcut *ui_removeShortcut;
};

#endif // OBJECTLISTMANAGERWIDGET_H
