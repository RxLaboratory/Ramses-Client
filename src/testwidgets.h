#ifndef TESTWIDGETS_H
#define TESTWIDGETS_H

#include "ui_testwidgets.h"

class TestWidgets : public QWidget, private Ui::TestWidgets
{
    Q_OBJECT

public:
    explicit TestWidgets(QWidget *parent = nullptr);

protected:
    void changeEvent(QEvent *e);
};

#endif // TESTWIDGETS_H
