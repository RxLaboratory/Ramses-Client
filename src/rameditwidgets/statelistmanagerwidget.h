#ifndef STATELISTMANAGERWIDGET_H
#define STATELISTMANAGERWIDGET_H

#include "objectlistmanagerwidget.h"

class StateListManagerWidget : public ObjectListManagerWidget<RamState*, int>
{
    Q_OBJECT
public:
    explicit StateListManagerWidget(QWidget *parent = nullptr);

protected slots:
    RamState *createObject() override;
};

#endif // STATELISTMANAGERWIDGET_H
