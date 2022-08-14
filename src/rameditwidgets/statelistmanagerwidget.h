#ifndef STATELISTMANAGERWIDGET_H
#define STATELISTMANAGERWIDGET_H

#include "objectlistmanagerwidget.h"
#include "ramstate.h"

class StateListManagerWidget : public ObjectListManagerWidget
{
    Q_OBJECT
public:
    explicit StateListManagerWidget(QWidget *parent = nullptr);

protected slots:
    RamState *createObject() override;
};

#endif // STATELISTMANAGERWIDGET_H
