#ifndef RAMSTATUSWIDGET_H
#define RAMSTATUSWIDGET_H

#include <QPlainTextEdit>

#include "duqf-widgets/duqfslider.h"

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

private slots:
    void remove();
    void statusChanged(RamObject *o);

private:
    RamStatus *_status;

    void completeUi();
    DuQFSlider *completionBox;
    QPlainTextEdit *commentEdit;

    bool _removing = false;
};

#endif // RAMSTATUSWIDGET_H