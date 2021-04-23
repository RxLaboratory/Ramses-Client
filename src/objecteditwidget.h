#ifndef OBJECTEDITWIDGET_H
#define OBJECTEDITWIDGET_H

#include <QScrollArea>

#include "ui_objecteditwidget.h"
#include "ramobject.h"

/**
 * @brief The ObjectEditWidget class is the base class of all editors for RamObjects (Shots, steps, etc)
 * It can be used either in the UI, or in a DockWidget
 */
class ObjectEditWidget : public QScrollArea, protected Ui::ObjectEditWidget
{
    Q_OBJECT

public:
    explicit ObjectEditWidget(QWidget *parent = nullptr);
    explicit ObjectEditWidget(RamObject *o, QWidget *parent = nullptr);

    RamObject *object() const;

    void hideName(bool hide = true);
    void hideStatus(bool hide = true);

public slots:
    void setObject(RamObject *object);

protected slots:
    virtual void update();
    virtual bool checkInput();

protected:
    QList<QMetaObject::Connection> _objectConnections;
    bool updating = false;

private slots:
    void objectRemoved(RamObject *o);
    void objectChanged(RamObject *o);

private:
    RamObject *_object;
    void connectEvents();
};

#endif // OBJECTEDITWIDGET_H
