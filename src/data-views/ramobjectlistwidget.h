#ifndef RAMOBJECTLISTWIDGET_H
#define RAMOBJECTLISTWIDGET_H

#include <QTableView>
#include <QHeaderView>
#include <QScrollBar>
#include <QSortFilterProxyModel>

#include "data-models/ramobjectlist.h"
#include "processmanager.h"
#include "ramobjectdelegate.h"
#include "data-models/ramitemfiltermodel.h"
#include "data-models/ramitemtablelistproxy.h"

/**
 * @brief The RamObjectListWidget class is the base class displaying for all lists in Ramses, displaying RamObject
 * It displays a RamObjectList using RamObjectDelegate for the painging.
 * It used mainly in ObjectListEditWidget (in order to manage the list)
 */
class RamObjectListWidget : public QTableView
{
    Q_OBJECT
public:
    enum EditMode { UnassignObjects, RemoveObjects };
    Q_ENUM( EditMode )
    enum DisplayMode { List, Table };
    Q_ENUM( DisplayMode )

    explicit RamObjectListWidget(DisplayMode mode = List, QWidget *parent = nullptr);
    explicit RamObjectListWidget(RamObjectList *list, DisplayMode mode = List, QWidget *parent = nullptr);
    explicit RamObjectListWidget(RamObjectList *list, bool editableObjects, RamUser::UserRole editRole = RamUser::Admin, DisplayMode mode = List, QWidget *parent = nullptr);
    // Content
    void setList(RamObjectList *list);
    RamItemFilterModel *filteredList();
    // Settings
    void setEditableObjects(bool editableObjects, RamUser::UserRole editRole = RamUser::Admin);
    void setSortable(bool sortable = true);

signals:
    void objectSelected(RamObject*);
    void editObject(RamObject*);
    void historyObject(RamObject*);

public slots:
    void search(QString s);
    void select(RamObject *o);
    void filter(RamObject *o);
    void setTimeTracking(bool trackTime);
    void setCompletionRatio(bool showCompletion);


protected:
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;
    virtual void showEvent(QShowEvent *event) override;

protected slots:
    // Relay to the objectSelected signal
    virtual void currentChanged(const QModelIndex &current, const QModelIndex &previous) override;
    // Moved
    void rowMoved( int logicalIndex, int oldVisualIndex, int newVisualIndex);

private slots:
    void revealFolder(RamObject *obj);

private:
    void setupUi();
    void connectEvents();

    RamItemFilterModel *m_objectList = nullptr;
    DisplayMode m_displayMode;

    // Delegate
    RamObjectDelegate *m_delegate;

    // Filters
    QString m_currentFilterUuid;

    // UI Events
    QPoint m_initialDragPos;
    bool m_dragging = false;
    bool m_layout = false;
};

#endif // RAMOBJECTLISTWIDGET_H
