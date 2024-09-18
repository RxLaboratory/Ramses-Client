#ifndef RAMJSONPROJECTEDITWIDGET_H
#define RAMJSONPROJECTEDITWIDGET_H

#include "ramjsonobjecteditwidget.h"

class RamJsonProjectEditWidget : public RamJsonObjectEditWidget
{
    Q_OBJECT
public:
    RamJsonProjectEditWidget(const QString &uuid = "", QWidget *parent = nullptr);

private:
    void setupUi();
    void connectEvents();
};

#endif // RAMJSONPROJECTEDITWIDGET_H
