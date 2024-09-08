#ifndef DULISTEDITVIEW_H
#define DULISTEDITVIEW_H

#include <QWidget>
#include <QToolButton>
#include <QAbstractItemModel>

#include "duwidgets/dulistview.h"
#include "duwidgets/dumenuview.h"

class DuListEditView : public QWidget
{
    Q_OBJECT
public:
    explicit DuListEditView(const QString &title, const QString &itemName, QAbstractItemModel *model, QWidget *parent = nullptr);
    void setAssignList(QAbstractItemModel *model);

signals:
    void editing(const QModelIndex &);

private slots:
    void add();
    void remove();
    void moveUp();
    void moveDown();
    void clear();
    void updateList();
    void assign(const QModelIndex &index);

private:
    void setupUi();
    void connectEvents();

    DuListView *ui_listView;
    QToolButton *ui_addButton;
    QToolButton *ui_removeButton;
    QToolButton *ui_moveUpButton;
    QToolButton *ui_moveDownButton;
    QToolButton *ui_clearButton;

    QAction *_addAction;
    DuMenuView *ui_assignMenu;

    QAbstractItemModel *_model;
    QAbstractItemModel *_assignList;

    QString _title;
    QString _itemName;
};

#endif // DULISTEDITVIEW_H
