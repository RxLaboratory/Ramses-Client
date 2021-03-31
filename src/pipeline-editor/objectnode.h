#ifndef OBJECTNODE_H
#define OBJECTNODE_H

#include <QDockWidget>

#include "duqf-nodeview/duqfnode.h"
#include "duqf-utils/guiutils.h"

#include "ramstep.h"
#include "objecteditwidget.h"
#include "objectdockwidget.h"
#include "stepeditwidget.h"

class ObjectNode : public DuQFNode
{
public:
    ObjectNode(RamObject *obj);
    ~ObjectNode();
    RamObject *ramObject() const;

    ObjectDockWidget *dockWidget() const;

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) Q_DECL_OVERRIDE;
    void setEditWidget(ObjectEditWidget *w);

private slots:
    void removeObject();
    virtual void objectChanged();
    void objectRemoved();

private:
    bool _hasEditWidget = false;
    RamObject *_object;
    ObjectDockWidget *_dockWidget;
};

#endif // STEPNODE_H
