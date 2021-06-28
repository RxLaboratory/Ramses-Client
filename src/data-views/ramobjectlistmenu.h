#ifndef RAMOBJECTLISTMENU_H
#define RAMOBJECTLISTMENU_H

#include <QMenu>
#include "data-models/ramobjectfiltermodel.h"

class RamObjectListMenu : public QMenu
{
    Q_OBJECT
public:
    RamObjectListMenu(bool checkable = false, QWidget *parent = nullptr);
    void setList(QAbstractItemModel *list);
    void addCreateButton();
    void setFilterUuid(const QString &uuid);

    void setObjectVisible(RamObject *obj, bool visible = true);

public slots:
    void selectAll();
    void selectNone();

signals:
    void create();
    void assign(RamObject*,bool);
    void assign(RamObject*);

private slots:
    void newObject(const QModelIndex &parent, int first, int last);
    void removeObject(const QModelIndex &parent, int first, int last);
    void objectChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int>());

    void actionAssign(bool checked);
    void actionAssign();
    void actionCreate();

private:
    RamObjectFilterModel *m_objects;
    RamObjectList *m_emptyList;
    bool m_checkable = false;
};

#endif // RAMOBJECTLISTMENU_H
