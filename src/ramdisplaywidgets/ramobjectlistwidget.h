#ifndef RAMOBJECTLISTWIDGET_H
#define RAMOBJECTLISTWIDGET_H

#include <QTableWidget>
#include <QHeaderView>

#include "ramobjectlist.h"
#include "ramobjectwidget.h"
#include "processmanager.h"

class RamObjectListWidget : public QTableWidget
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
    void setContainingType(RamObject::ObjectType type);
    void setEditMode(const EditMode &editMode);
    void setList(RamObjectList *list);
    void addList(RamObjectList *list);
    void setList(RamObjectUberList *list);
    void clear();
    void setSortable(bool sortable=true);
    void setSelectable(bool selectable=true);

    void select(RamObject *obj);

public slots:
    void removeSelectedObjects();
    void search(QString nameOrShortName);
    void filter(QString uuid);

signals:
    void itemDropped();
    void objectSelected(RamObject*);
    void orderChanged();

protected:
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
    void showEvent(QShowEvent *event) Q_DECL_OVERRIDE;

private slots:
    void itemSelected(QTableWidgetItem *current, QTableWidgetItem *previous);
    void changeSelection();
    void updateOrder();

    void objectChanged(RamObject *obj);
    void objectUnassigned(RamObject *obj);
    void objectAssigned(RamObject *obj);
private:
    QList<RamObjectList *> m_lists;
    EditMode m_editMode = RemoveObjects;

    void setupUi();
    void connectEvents();

    bool m_editableObjects = false;
    QList<QMetaObject::Connection> m_listConnections;
    QMap<QString, QMetaObject::Connection> m_objectConnections;
    RamObjectUberList *m_uberList = nullptr;

    RamObject::ObjectType m_containingType = RamObject::Generic;

    QList<RamObjectList *> m_listsToAdd;
    void addLists();
};

#endif // RAMOBJECTLISTWIDGET_H
