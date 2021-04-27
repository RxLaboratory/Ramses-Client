#include "statelistmanagerwidget.h"

StateListManagerWidget::StateListManagerWidget(QWidget *parent) :
    ObjectListManagerWidget(
        Ramses::instance()->states(),
        new StateEditWidget(),
        parent)
{

}

void StateListManagerWidget::createObject()
{
    Ramses::instance()->createState();
}

