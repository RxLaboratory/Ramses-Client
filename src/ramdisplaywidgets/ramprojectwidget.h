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

protected slots:
    void exploreClicked() Q_DECL_OVERRIDE;

private:
    RamProject *m_project;

    void completeUi();
};

#endif // RAMPROJECTWIDGET_H
