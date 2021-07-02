#ifndef STATEBOX_H
#define STATEBOX_H

#include <QComboBox>
#include <QPaintEvent>
#include <QStylePainter>

#include "ramses.h"
#include "data-views/ramobjectlistcombobox.h"

class StateBox : public RamObjectListComboBox
{
    Q_OBJECT
public:
    StateBox(QWidget *parent = nullptr);

protected:

private slots:
    void currentStateChanged(RamObject *obj);

};

#endif // STATEBOX_H
