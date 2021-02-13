#include "testwidgets.h"

TestWidgets::TestWidgets(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);
}

void TestWidgets::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi(this);
        break;
    default:
        break;
    }
}
