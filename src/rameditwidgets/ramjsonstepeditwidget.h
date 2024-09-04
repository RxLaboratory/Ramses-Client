#ifndef RAMJSONSTEPEDITWIDGET_H
#define RAMJSONSTEPEDITWIDGET_H

#include <QSplitter>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QToolButton>
#include <QFormLayout>

#include "duwidgets/ducombobox.h"
#include "ramjsonobjecteditwidget.h"

class RamJsonStepEditWidget: public RamJsonObjectEditWidget
{
    Q_OBJECT
public:
    RamJsonStepEditWidget(QWidget *parent = nullptr);

private:
    void setupUi();
    void connectEvents();

};

#endif // RAMJSONSTEPEDITWIDGET_H
