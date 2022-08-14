#ifndef STATEBOX_H
#define STATEBOX_H

#include <QComboBox>
#include <QPaintEvent>
#include <QStylePainter>

#include "data-views/ramobjectlistcombobox.h"
#include "ramstate.h"

class StateBox : public RamObjectListComboBox
{
    Q_OBJECT
public:
    StateBox(QWidget *parent = nullptr);

protected:

private slots:
    void currentStateChanged(RamState *state);

};

#endif // STATEBOX_H
