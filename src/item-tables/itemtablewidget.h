#ifndef ITEMTABLEWIDGET_H
#define ITEMTABLEWIDGET_H

#include <QTableWidget>
#include <QHeaderView>
#include <QMouseEvent>

#include "ramobjectwidget.h"
#include "ramobjectwidget.h"

class ItemTableWidget : public QTableWidget
{
    Q_OBJECT
public:
    ItemTableWidget(QWidget *parent = nullptr);
    void setList(RamObjectUberList *list, RamStep::Type stepType = RamStep::AssetProduction);
    void clear();
    void setSortable(bool sortable);
    void addList(RamObjectList *list);
    void setStepVisible(QString stepUuid, bool visible);

public slots:
    void search(QString s);

signals:
    void newStep(RamStep *);
    void stepRemoved(RamObject *);

protected slots:
    virtual void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    virtual void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    virtual void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

private slots:
    void objectChanged(RamObject *obj);
    void objectUnassigned(RamObject *obj);
    void objectAssigned(RamObject *obj);
    int addStep(RamStep *step);
    void removeStep(RamObject *stepObj);
    void stepChanged(RamObject *stepObj);
    void statusAdded(RamObject *statusObj, int index=0);
    void stepStatusHistoryAdded(RamObject *sshObj);
    void stepStatusHistoryRemoved(RamObject *sshObj);
    void statusRemoved(RamObject *statusObj);

private:
    void setupUi();
    void connectEvents();

    void disconnectObject(RamObject *obj);
    void disconnectStep(QString stepUuid);

    void setStatusWidget(RamItem *item, RamStep *step);
    RamStatus *generateDefaultStatus(RamItem *item, RamStep *step);

    QList<QMetaObject::Connection> m_listConnections;
    QMap<QString, QList<QMetaObject::Connection>> m_objectConnections;
    QMap<QString, QList<QMetaObject::Connection>> m_stepConnections;
    QList<RamObjectList *> m_lists;
    RamObjectUberList *m_uberList = nullptr;
    RamStep::Type m_stepType = RamStep::AssetProduction;

    //Drag
    QPoint _initialDragPos;
    bool _dragging = false;
};

#endif // ITEMTABLEWIDGET_H
