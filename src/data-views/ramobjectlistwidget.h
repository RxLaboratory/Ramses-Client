#ifndef RAMOBJECTLISTWIDGET_H
#define RAMOBJECTLISTWIDGET_H

#include <QTableView>
#include <QHeaderView>
#include <QScrollBar>
#include <QSortFilterProxyModel>

#include "data-models/ramobjectlist.h"
#include "processmanager.h"
#include "ramobjectdelegate.h"
#include "data-models/ramobjectfiltermodel.h"

class RamObjectListWidget : public QTableView
{
    Q_OBJECT
public:
    enum EditMode { UnassignObjects, RemoveObjects };
    Q_ENUM( EditMode )

    explicit RamObjectListWidget(QWidget *parent = nullptr);
    explicit RamObjectListWidget(RamObjectList *list, QWidget *parent = nullptr);
    explicit RamObjectListWidget(RamObjectList *list, bool editableObjects, RamUser::UserRole editRole = RamUser::Admin, QWidget *parent = nullptr);
    // Content
    void setList(RamObjectList *list);

signals:
    void objectSelected(RamObject*);
    void editObject(RamObject*);

public slots:
    void search(QString s);
    void select(RamObject *o);
    void filter(RamObject *o);

protected:
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;

protected slots:
    // Relay to the objectSelected signal
    virtual void currentChanged(const QModelIndex &current, const QModelIndex &previous) Q_DECL_OVERRIDE;

private:
    void setupUi();
    void connectEvents();

    RamObjectFilterModel *m_objectList = nullptr;

    // Delegate
    RamObjectDelegate *m_delegate;

    // Filters
    QString m_currentFilterUuid;

    // UI Events
    QPoint _initialDragPos;
    bool m_dragging = false;
};

#endif // RAMOBJECTLISTWIDGET_H
