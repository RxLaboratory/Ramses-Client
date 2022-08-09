#ifndef OBJECTLISTMANAGERWIDGET_H
#define OBJECTLISTMANAGERWIDGET_H

#include <QWidget>
#include <QSplitter>

#include "objectlisteditwidget.h"

template<typename RO, typename ROF> class ObjectListManagerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ObjectListManagerWidget(QString title, QIcon icon = QIcon(), QWidget *parent = nullptr);
    explicit ObjectListManagerWidget(RamObjectList<RO> *objectList, QString title, QIcon icon = QIcon(), QWidget *parent = nullptr);
    void setList(RamObjectList<RO> *objectList);
    void clear();
    QString currentFilterUuid() const;
    ROF currentFilter() const;

    QToolButton *menuButton();

protected:
    ObjectListEditWidget<RO, ROF> *m_listEditWidget;

protected slots:
    virtual RO createObject() { return nullptr; };
    void createEditObject();

private:
    void setupUi(QString title, QIcon icon);
    void connectEvents();

    QToolButton *ui_itemButton;
    QAction *ui_createAction;

    QShortcut *ui_createShortcut;
    QShortcut *ui_removeShortcut;
};

#endif // OBJECTLISTMANAGERWIDGET_H
