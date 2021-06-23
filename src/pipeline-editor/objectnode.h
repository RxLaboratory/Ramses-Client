#ifndef OBJECTNODE_H
#define OBJECTNODE_H

#include <QDockWidget>

#include "duqf-nodeview/duqfnode.h"
#include "duqf-utils/guiutils.h"

#include "ramobject.h"
#include "objecteditwidget.h"
#include "objectdockwidget.h"

class ObjectNode : public DuQFNode
{
public:
    ObjectNode(RamObject *obj);
    RamObject *ramObject() const;

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) Q_DECL_OVERRIDE;

private slots:
    void removeObject();
    virtual void objectChanged();
    void objectRemoved();

private:
    RamObject *m_object;
};

#endif // STEPNODE_H
