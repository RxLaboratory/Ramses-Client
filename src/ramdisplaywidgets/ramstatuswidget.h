#ifndef RAMSTATUSWIDGET_H
#define RAMSTATUSWIDGET_H

#include <QPlainTextEdit>
#include <QSettings>

#include "duqf-widgets/duqfslider.h"

#include "ramobjectwidget.h"
#include "ramses.h"
#include "statuseditwidget.h"
#include "stepstatushistorywidget.h"

/**
 * @brief The RamStatusWidget class is a small widget used to show a Status anywhere in the UI
 */
class RamStatusWidget : public RamObjectWidget
{
    Q_OBJECT
public:
    RamStatusWidget(RamStatus *status, QWidget *parent = nullptr);
    RamStatus *status() const;
    void showHistoryButton();
    // Adds a new status instead of editing the current one
    void setAdditiveMode();

protected slots:
    void exploreClicked() Q_DECL_OVERRIDE;
    virtual void objectChanged() Q_DECL_OVERRIDE;

private slots:
    void updateStatus(RamState*state, int completion, int version, QString comment);
    void showHistory();

private:
    RamStatus *_status;

    void completeUi();
    void connectEvents();

    DuQFSlider *completionBox;
    QLabel *userLabel;
    StatusEditWidget *statusEditWidget;
    StepStatusHistoryWidget *m_historyWidget = nullptr;
    ObjectDockWidget *m_historyDockWidget = nullptr;
    QToolButton *m_historyButton;

    bool m_additiveMode = false;

    bool m_historyBuilt = false;
};

#endif // RAMSTATUSWIDGET_H
