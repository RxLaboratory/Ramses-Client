#ifndef DULISTMODELEDIT_H
#define DULISTMODELEDIT_H

#include <QWidget>
#include <QToolButton>
#include <QAbstractItemModel>

#include "duwidgets/dulistview.h"

class DuListModelEdit : public QWidget
{
    Q_OBJECT
public:
    explicit DuListModelEdit(const QString &title, QAbstractItemModel *model, QWidget *parent = nullptr);

signals:
    void editing(const QModelIndex &);

private slots:
    void add();
    void remove();
    void moveUp();
    void moveDown();
    void clear();
    void updateList();

private:
    void setupUi(const QString &title);
    void connectEvents();

    DuListView *ui_listView;
    QToolButton *ui_addButton;
    QToolButton *ui_removeButton;
    QToolButton *ui_moveUpButton;
    QToolButton *ui_moveDownButton;
    QToolButton *ui_clearButton;

    QAbstractItemModel *_model;
    QAbstractItemModel *_assignList;
};

#endif // DULISTMODELEDIT_H
