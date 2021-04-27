#ifndef STATELISTMANAGERWIDGET_H
#define STATELISTMANAGERWIDGET_H

#include "objectlistmanagerwidget.h"
#include "stateeditwidget.h"
#include "ramses.h"

class StateListManagerWidget : public ObjectListManagerWidget
{
    Q_OBJECT
public:
    explicit StateListManagerWidget(QWidget *parent = nullptr);

protected slots:
    void createObject() Q_DECL_OVERRIDE;
};

#endif // STATELISTMANAGERWIDGET_H
