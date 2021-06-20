#ifndef RAMOBJECTLISTWIDGET_H
#define RAMOBJECTLISTWIDGET_H

#include <QTableView>
#include <QHeaderView>
#include <QSortFilterProxyModel>

#include "data-models/ramobjectlist.h"
#include "processmanager.h"
#include "ramobjectitemdelegate.h"

class RamObjectListWidget : public QTableView
{
    Q_OBJECT
public:
    enum EditMode { UnassignObjects, RemoveObjects };
    Q_ENUM( EditMode )

    explicit RamObjectListWidget(QWidget *parent = nullptr);
    explicit RamObjectListWidget(RamObjectList *list, QWidget *parent = nullptr);
    explicit RamObjectListWidget(RamObjectList *list, bool editableObjects, QWidget *parent = nullptr);

    // Settings
    void setContainingType(RamObject::ObjectType type) {};
    void setEditMode(const EditMode &editMode) {};
    void setSortable(bool sortable=true) {};
    void setSelectable(bool selectable=true) {};

    // Content
    void setList(RamObjectList *list);
    void clear() {};

public slots:
    void select(RamObject *obj) {};
    void removeSelectedObjects() {};
    void search(QString nameOrShortName) {};
    void filter(QString uuid) {};

signals:
    void itemDropped();
    void objectSelected(RamObject*);
    void orderChanged();

private:
    void setupUi();
};

#endif // RAMOBJECTLISTWIDGET_H
