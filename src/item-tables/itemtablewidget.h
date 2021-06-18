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
    // Settings
    void setSortable(bool sortable);
    // Content
    void setList(RamObjectUberList *list, RamStep::Type stepType = RamStep::AssetProduction);
    void clear();

public slots:
    void search(QString s);
    void setStepVisible(QString stepUuid, bool visible);

signals:
    void newStep(RamStep *);
    void stepRemoved(RamObject *);

protected:
    virtual void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    virtual void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    virtual void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void showEvent(QShowEvent *event) Q_DECL_OVERRIDE;


private slots:
    // Items
    void objectChanged(RamObject *obj);
    void objectUnassigned(RamObject *obj);
    void objectAssigned(RamObject *obj);
    // Steps
    int addStep(RamStep *step);
    void removeStep(RamObject *stepObj);
    void stepChanged(RamObject *stepObj);
    // Status
    void statusAdded(RamObject *statusObj, int index=0);
    void stepStatusHistoryAdded(RamObject *sshObj);
    void stepStatusHistoryRemoved(RamObject *sshObj);
    void statusRemoved(RamObject *statusObj);

private:
    // The List
    RamObjectUberList *m_uberList = nullptr;

    // The Connections
    QList<QMetaObject::Connection> m_listConnections;
    QMap<QString, QList<QMetaObject::Connection>> m_objectConnections;
    QMap<QString, QList<QMetaObject::Connection>> m_stepConnections;
    void disconnectObject(RamObject *obj);
    void disconnectStep(QString stepUuid);

    // Settings
    RamStep::Type m_stepType = RamStep::AssetProduction;

    // Build
    void setupUi();
    void connectEvents();
    // Delayed build for perf
    bool m_ready = false;
    void addList();

    // Utils
    void setStatusWidget(RamItem *item, RamStep *step);
    RamStatus *generateDefaultStatus(RamItem *item, RamStep *step);
    QString objUuid(int row);
    QString objShortName(int row);
    QString objName(int row);
    int objRow(RamObject *o);
    QString stepUuid(int col);
    int stepColumn(QString stepUuid);
    int stepColumn(RamObject *stepObj);

    // UI Events
    QPoint _initialDragPos;
    bool _dragging = false;
};

#endif // ITEMTABLEWIDGET_H
