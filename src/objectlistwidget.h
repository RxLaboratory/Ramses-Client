#ifndef OBJECTLISTWIDGET_H
#define OBJECTLISTWIDGET_H

#include <QTableWidget>
#include <QHeaderView>

#include "ramobjectwidget.h"
#include "ramassetwidget.h"
#include "ramshotwidget.h"
#include "ramstatuswidget.h"

class ObjectListWidget : public QTableWidget
{
    Q_OBJECT
public:
    ObjectListWidget(QWidget *parent = nullptr);
    ObjectListWidget(bool editableObjects, QWidget *parent = nullptr);
    void setDragable(bool dragable=true);

    void addObject(RamObject *obj, bool edit=false);

    QList<RamObject*> objects() const;

public slots:
    void removeObject(RamObject *obj);
    void removeObject(QString uuid);
    void removeSelectedObjects();
signals:
    void itemDropped();
    void objectSelected(RamObject*);
    void objectRemoved(RamObject*);
    void orderChanged();
protected:
    void dropEvent(QDropEvent *event) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
private slots:
    void itemSelected(QTableWidgetItem *previous, QTableWidgetItem *current);
    void changeSelection();
    void updateOrder();
private:
    void setupUi();
    void connectEvents();
    bool m_editableObjects = false;
};

#endif // OBJECTLISTWIDGET_H
