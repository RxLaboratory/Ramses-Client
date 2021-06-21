#ifndef RAMOBJECTLISTWIDGET_H
#define RAMOBJECTLISTWIDGET_H

#include <QTableView>
#include <QHeaderView>
#include <QScrollBar>
#include <QSortFilterProxyModel>

#include "data-models/ramobjectlist.h"
#include "processmanager.h"
#include "ramobjectdelegate.h"

class RamObjectListWidget : public QTableView
{
    Q_OBJECT
public:
    enum EditMode { UnassignObjects, RemoveObjects };
    Q_ENUM( EditMode )

    explicit RamObjectListWidget(QWidget *parent = nullptr);
    explicit RamObjectListWidget(RamObjectList *list, QWidget *parent = nullptr);
    explicit RamObjectListWidget(RamObjectList *list, bool editableObjects, QWidget *parent = nullptr);
    // Content
    void setList(RamObjectList *list);

signals:
    void objectSelected(RamObject*) const;

public slots:
    void search(QString s);
    void select(RamObject *o);

protected:
    virtual void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    virtual void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    virtual void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    virtual void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

protected slots:
    // Relay to the objectSelected signal
    virtual void currentChanged(const QModelIndex &current, const QModelIndex &previous) Q_DECL_OVERRIDE;

private:
    void setupUi();
    void connectEvents();

    RamObjectList *m_objectList = nullptr;

    // UI Events
    QPoint _initialDragPos;
    bool m_dragging = false;
};

#endif // RAMOBJECTLISTWIDGET_H
