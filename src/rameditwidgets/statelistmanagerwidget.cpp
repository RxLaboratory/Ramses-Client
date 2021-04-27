#include "statelistmanagerwidget.h"

StateListManagerWidget::StateListManagerWidget(QWidget *parent) :
    ObjectListManagerWidget(
        Ramses::instance()->states(),
        new StateEditWidget(),
        "States",
        parent)
{

}

void StateListManagerWidget::createObject()
{
    Ramses::instance()->createState();
}

