#ifndef STATESMANAGERWIDGET_H
#define STATESMANAGERWIDGET_H

#include "listmanagerwidget.h"
#include "stateeditwidget.h"

class StatesManagerWidget : public ListManagerWidget
{
    Q_OBJECT
public:
    StatesManagerWidget(QWidget *parent = nullptr);

protected slots:
    void currentDataChanged(QVariant data) Q_DECL_OVERRIDE;
    void createItem() Q_DECL_OVERRIDE;
    void removeItem(QVariant data) Q_DECL_OVERRIDE;

private slots:
    void newState(RamState *state);
    void stateRemoved(RamObject *state);
    void stateChanged();

private:
    StateEditWidget *stateWidget;
};

#endif // STATESMANAGERWIDGET_H
