#ifndef RAMAPPLICATIONWIDGET_H
#define RAMAPPLICATIONWIDGET_H

#include "ramobjectwidget.h"
#include "applicationeditwidget.h"

class RamApplicationWidget : public RamObjectWidget
{
    Q_OBJECT
public:
    RamApplicationWidget(RamApplication *application, QWidget *parent = nullptr);
    RamApplication *application() const;

private:
    RamApplication *m_application;
};

#endif // RAMAPPLICATIONWIDGET_H
