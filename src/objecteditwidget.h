#ifndef OBJECTEDITWIDGET_H
#define OBJECTEDITWIDGET_H

#include <QScrollArea>

#include "ui_objecteditwidget.h"
#include "ramobject.h"

class ObjectEditWidget : public QScrollArea, protected Ui::ObjectEditWidget
{
    Q_OBJECT

public:
    explicit ObjectEditWidget(QWidget *parent = nullptr);
    explicit ObjectEditWidget(RamObject *o, QWidget *parent = nullptr);

    RamObject *object() const;
    void setObject(RamObject *object);

protected slots:
    virtual void update();
    void revert();
    virtual bool checkInput();

protected:
    QList<QMetaObject::Connection> _objectConnections;

private slots:
    void objectRemoved(RamObject *o);

private:
    RamObject *_object;
    void connectEvents();
};

#endif // OBJECTEDITWIDGET_H
