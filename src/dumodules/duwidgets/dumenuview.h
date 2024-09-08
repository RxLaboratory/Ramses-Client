#ifndef DUMENUVIEW_H
#define DUMENUVIEW_H

#include "dumenu.h"
#include "qabstractitemmodel.h"

class DuMenuView : public DuMenu
{
    Q_OBJECT
public:
    DuMenuView(const QString &title, QWidget *parent = nullptr);

    QAbstractItemModel *model() const;
    void setModel(QAbstractItemModel *newModel);

signals:
    void indexTriggered(const QModelIndex &);

private:
    void clear();
    void rebuild();
    void updateData(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int>());

    QAbstractItemModel *_model = nullptr;
    QVector<QAction *> _actions;
};

#endif // DUMENUVIEW_H
