#ifndef OBJECTNODE_H
#define OBJECTNODE_H

#include <QDockWidget>

#include "duqf-nodeview/duqfnode.h"
#include "duqf-utils/guiutils.h"

#include "ramobject.h"
#include "objecteditwidget.h"

class ObjectNode : public DuQFNode
{
public:
    ObjectNode(RamObject *obj);
    RamObject *ramObject() const;

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

private slots:
    void removeObject();
    virtual void objectChanged();
    void objectRemoved();

private:
    RamObject *m_object;

    bool m_mousePress;
};

#endif // STEPNODE_H
