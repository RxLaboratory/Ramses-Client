#ifndef STATEBOX_H
#define STATEBOX_H

#include <QComboBox>

#include "ramses.h"

class StateBox : public QComboBox
{
    Q_OBJECT
public:
    StateBox(QWidget *parent = nullptr);
    void setCurrentState(RamState *state);
    void setCurrentState(QString shortName);
    RamState *currentState() const;
signals:
    void currentStateChanged(RamState *);
private slots:
    void currentStateChanged(int i);
    void newState(RamState *state);
    void stateRemoved(RamObject *o);
    void stateChanged(RamObject *o);
};

#endif // STATEBOX_H
