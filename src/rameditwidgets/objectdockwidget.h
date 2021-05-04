#ifndef OBJECTDOCKWIDGET_H
#define OBJECTDOCKWIDGET_H

#include <QDockWidget>

#include "duqf-widgets/duqfdocktitle.h"
#include "duqf-app/app-utils.h"

#include "ramobject.h"

class ObjectDockWidget : public QDockWidget
{
    Q_OBJECT
public:
    ObjectDockWidget(RamObject *obj, QWidget *parent = nullptr);

    RamObject::ObjectType objectType() const;

    void setTitle(QString title);
    void setIcon(QString icon);

    bool pinned() const;

public slots:
    void pin(bool p);

private slots:
    void objectChanged(RamObject *o);
    void objectRemoved(RamObject *o);

private:
    RamObject *_object;

    bool _pinned = false;

    DuQFDockTitle *_titleWidget;
};

#endif // OBJECTDOCKWIDGET_H
