#ifndef ITEMTABLEWIDGET_H
#define ITEMTABLEWIDGET_H

#include <QTableWidget>
#include <QHeaderView>

#include "ramobjectwidget.h"
#include "ramobjectwidget.h"

class ItemTableWidget : public QTableWidget
{
    Q_OBJECT
public:
    ItemTableWidget(QWidget *parent = nullptr);
    void setList(RamObjectUberList *list, RamObjectList *steps, RamStep::Type stepType = RamStep::AssetProduction);
    void clear();
    void setSortable(bool sortable);
    void addList(RamObjectList *list);


private slots:
    void objectChanged(RamObject *obj);
    void objectUnassigned(RamObject *obj);
    void objectAssigned(RamObject *obj);
    void addStep(RamObject *stepObj);
    void removeStep(RamObject *stepObj);
    void stepChanged(RamObject *stepObj);

private:
    void setupUi();
    void connectEvents();

    void disconnectObject(RamObject *obj);
    void disconnectStep(QString stepUuid);

    QList<QMetaObject::Connection> m_listConnections;
    QMap<QString, QMetaObject::Connection> m_objectConnections;
    QMap<QString, QMetaObject::Connection> m_stepConnections;
    QList<RamObjectList *> m_lists;
    RamObjectUberList *m_uberList = nullptr;
    RamStep::Type m_stepType = RamStep::AssetProduction;
};

#endif // ITEMTABLEWIDGET_H
