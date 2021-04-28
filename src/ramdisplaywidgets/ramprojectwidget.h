#ifndef RAMPROJECTWIDGET_H
#define RAMPROJECTWIDGET_H

#include "ramobjectwidget.h"
#include "ramproject.h"
#include "projecteditwidget.h"

class RamProjectWidget : public RamObjectWidget
{
    Q_OBJECT
public:
    RamProjectWidget(RamProject *project, QWidget *parent = nullptr);

private:
    RamProject *_project;
};

#endif // RAMPROJECTWIDGET_H
