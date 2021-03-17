#ifndef STEPNODE_H
#define STEPNODE_H

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
    RamObject *ramObject() const;

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) Q_DECL_OVERRIDE;

private slots:
    virtual void objectChanged();
    void objectRemoved();

private:
    RamObject *_object;
    ObjectDockWidget *_dockWidget;
};

#endif // STEPNODE_H
