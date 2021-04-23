#ifndef RAMSTATUSWIDGET_H
#define RAMSTATUSWIDGET_H

#include "duqf-widgets/duqfspinbox.h"

#include "ramobjectwidget.h"

#include "ramses.h"

/**
 * @brief The RamStatusWidget class is a small widget used to show a Status anywhere in the UI
 */
class RamStatusWidget : public RamObjectWidget
{
    Q_OBJECT
public:
    RamStatusWidget(RamStatus *status, QWidget *parent = nullptr);
    RamStatus *status() const;

    void setEdibable(bool editable=true);

private slots:
    void remove();
    void statusChanged(RamObject *o);

private:
    RamStatus *_status;

    void completeUi();
    DuQFSpinBox *completionBox;

    bool _removing = false;
};

#endif // RAMSTATUSWIDGET_H
