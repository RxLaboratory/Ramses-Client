#ifndef RAMOBJECTLISTWIDGET_H
#define RAMOBJECTLISTWIDGET_H

#include <QTableWidget>
#include <QHeaderView>

#include "ramobjectlist.h"
#include "ramobjectwidget.h"
#include "ramassetwidget.h"
#include "ramshotwidget.h"
#include "ramstatuswidget.h"

class RamObjectListWidget : public QTableWidget
{
    Q_OBJECT
public:
    explicit RamObjectListWidget(RamObjectList *list, QWidget *parent = nullptr);
    explicit RamObjectListWidget(RamObjectList *list, bool editableObjects, QWidget *parent = nullptr);
    void setSortable(bool sortable=true);
    void setSelectable(bool selectable=true);

    RamObjectList *objects() const;

public slots:
    void removeSelectedObjects(bool removeFromListOnly = false);

signals:
    void itemDropped();
    void objectSelected(RamObject*);
    void objectRemoved(RamObject*);
    void orderChanged();

private slots:
    void itemSelected(QTableWidgetItem *previous, QTableWidgetItem *current);
    void changeSelection();
    void updateOrder();
    void objectChanged(RamObject *obj);

    void removeObject(RamObject *obj);
    void addObject(RamObject *obj);
private:
    RamObjectList *m_list;

    void setupUi();
    void connectEvents();

    bool m_editableObjects = false;
    QMap<QString, QMetaObject::Connection> m_objectConnections;
};

#endif // RAMOBJECTLISTWIDGET_H
