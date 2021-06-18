#ifndef RAMOBJECTLISTWIDGET_H
#define RAMOBJECTLISTWIDGET_H

#include <QListView>
#include <QHeaderView>
#include <QSortFilterProxyModel>

#include "ramobjectlist.h"
#include "ramobjectwidget.h"
#include "processmanager.h"
#include "ramobjectitemdelegate.h"

#include "ramobjectlistmodel.h"

class RamObjectListWidget : public QListView
{
    Q_OBJECT
public:
    enum EditMode { UnassignObjects, RemoveObjects };
    Q_ENUM( EditMode )

    explicit RamObjectListWidget(QWidget *parent = nullptr);
    explicit RamObjectListWidget(RamObjectList *list, QWidget *parent = nullptr);
    explicit RamObjectListWidget(RamObjectUberList *list, QWidget *parent = nullptr);
    explicit RamObjectListWidget(RamObjectList *list, bool editableObjects, QWidget *parent = nullptr);
    explicit RamObjectListWidget(RamObjectUberList *list, bool editableObjects, QWidget *parent = nullptr);

    // Settings
    void setContainingType(RamObject::ObjectType type) {};
    void setEditMode(const EditMode &editMode) {};
    void setSortable(bool sortable=true) {};
    void setSelectable(bool selectable=true) {};

    // Content
    void setList(RamObjectList *list) {};
    void setList(RamObjectUberList *list) {};
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
