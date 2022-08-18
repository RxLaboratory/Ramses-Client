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

signals:
    void currentStateChanged(RamState *state);

private slots:
    void changeCurrentState(RamObject *state);

};

#endif // STATEBOX_H
