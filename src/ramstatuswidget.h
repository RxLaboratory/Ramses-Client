#ifndef RAMSTATUSWIDGET_H
#define RAMSTATUSWIDGET_H

#include <QPlainTextEdit>
#include <QSettings>

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

protected:
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
    void showEvent(QShowEvent *event) Q_DECL_OVERRIDE;

private slots:
    void remove();
    void statusChanged(RamObject *o);
    void adjustCommentEditSize();

private:
    RamStatus *_status;

    void completeUi();
    void connectEvents();
    DuQFSlider *completionBox;
    QPlainTextEdit *commentEdit;
    QLabel *userLabel;

    bool _removing = false;
};

#endif // RAMSTATUSWIDGET_H
