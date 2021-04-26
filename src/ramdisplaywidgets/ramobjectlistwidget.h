#ifndef RAMOBJECTLISTWIDGET_H
#define RAMOBJECTLISTWIDGET_H

#include <QTableWidget>
#include <QHeaderView>

#include "ramobjectlist.h"
#include "ramobjectwidget.h"
#include "ramassetwidget.h"
#include "ramshotwidget.h"
#include "ramstatuswidget.h"
#include "ramuserwidget.h"

class RamObjectListWidget : public QTableWidget
{
    Q_OBJECT
public:
    explicit RamObjectListWidget(RamObjectList *list, QWidget *parent = nullptr);
    explicit RamObjectListWidget(RamObjectList *list, bool editableObjects, QWidget *parent = nullptr);
    void setSortable(bool sortable=true);
    void setSelectable(bool selectable=true);

    RamObjectList *objects() const;

    void select(RamObject *obj);

public slots:
    void removeSelectedObjects();
    void unassignSelectedObjects();

signals:
    void itemDropped();
    void objectSelected(RamObject*);
    void orderChanged();

protected:
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

private slots:
    void itemSelected(QTableWidgetItem *current, QTableWidgetItem *previous);
    void changeSelection();
    void updateOrder();

    void objectChanged(RamObject *obj);
    void objectUnassigned(RamObject *obj);
    void objectAssigned(RamObject *obj);
private:
    RamObjectList *m_list;

    void setupUi();
    void connectEvents();

    bool m_editableObjects = false;
    QMap<QString, QMetaObject::Connection> m_objectConnections;
};

#endif // RAMOBJECTLISTWIDGET_H
