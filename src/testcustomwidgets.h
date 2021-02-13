#ifndef TESTCUSTOMWIDGETS_H
#define TESTCUSTOMWIDGETS_H

#include "ui_testcustomwidgets.h"

#include "duqf-widgets/duqfspinbox.h"
#include "duqf-widgets/duqfdoublespinbox.h"
#include "duqf-widgets/duqfslider.h"
#include "duqf-widgets/duqfdoubleslider.h"

class TestCustomWidgets : public QWidget, private Ui::TestCustomWidgets
{
    Q_OBJECT

public:
    explicit TestCustomWidgets(QWidget *parent = nullptr);
};

#endif // TESTCUSTOMWIDGETS_H
