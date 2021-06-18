#ifndef RAMOBJECTLISTWIDGET_BAK_H
#define RAMOBJECTLISTWIDGET_BAK_H

#include <QTableWidget>
#include <QHeaderView>

#include "ramobjectlist.h"
#include "ramobjectwidget.h"
#include "processmanager.h"

class RamObjectTableItem : public QTableWidgetItem
{
public:
    RamObjectTableItem(RamObject*o);
    virtual bool operator<(const QTableWidgetItem &other) const;
};

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
    // Settings
    void setContainingType(RamObject::ObjectType type);
    void setEditMode(const EditMode &editMode);
    void setSortable(bool sortable=true);
    void setSelectable(bool selectable=true);
    // Content
    void setList(RamObjectList *list);
    void setList(RamObjectUberList *list);
    void clear();

public slots:
    void select(RamObject *obj);
    void removeSelectedObjects();
    void search(QString nameOrShortName);
    void filter(QString uuid);

signals:
    void itemDropped();
    void objectSelected(RamObject*);
    void orderChanged();

protected:
    virtual void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    virtual void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    virtual void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    virtual void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
    virtual void showEvent(QShowEvent *event) Q_DECL_OVERRIDE;

private slots:
    void itemSelected(QTableWidgetItem *current, QTableWidgetItem *previous);
    void changeSelection();
    void updateOrder();
    void listSorted();

    void objectChanged(RamObject *obj);
    void objectUnassigned(RamObject *obj);
    void objectAssigned(RamObject *obj);

private:
    // The (uber) list
    RamObjectList *m_list = nullptr;
    RamObjectUberList *m_uberList = nullptr;

    // The connections
    QList<QMetaObject::Connection> m_listConnections;
    QMap<QString, QMetaObject::Connection> m_objectConnections;

    // Settings
    EditMode m_editMode = RemoveObjects;
    bool m_editableObjects = false;
    RamObject::ObjectType m_containingType = RamObject::Generic;

    // Build
    void setupUi();
    void connectEvents();
    // Delayed build for perf
    bool m_ready = false;
    void addLists();
    bool m_sorting = false;

    // Utils
    QString objUuid(int row);
    QString objShortName(int row);
    QString objName(int row);
    RamObject *objAtRow(int row);
    int objRow(RamObject *o);

    // UI Events
    QPoint _initialDragPos;
    bool _dragging = false;
};

#endif // RAMOBJECTLISTWIDGET_BAK_H
