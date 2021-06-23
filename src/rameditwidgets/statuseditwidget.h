#ifndef STATUSEDITWIDGET_H
#define STATUSEDITWIDGET_H

#include <QPlainTextEdit>
#include <QToolButton>

#include "duqf-widgets/duqfspinbox.h"
#include "duqf-widgets/autoselectspinbox.h"
#include "objecteditwidget.h"
#include "statebox.h"
#include "ramses.h"

/**
 * @brief The StatusEditWidget class is used to edit a RamStatus.
 * IT DOES NOT update the status by itself (so it can be used either to edit an existing one, or get a new status).
 * The 'statusUpdated' signal is sent when the status needs to be updated.
 * The 'setStatus' method is used to show an existing status information, but the status itself won't be changed.
 */
class StatusEditWidget : public ObjectEditWidget
{
    Q_OBJECT
public:
    StatusEditWidget(QWidget *parent = nullptr);
    StatusEditWidget(RamStatus *status, QWidget *parent = nullptr);
    /**
     * @brief setStatus Use this method to set all fields corresponding to an existing status
     * Note that this Widget WILL NOT change the status you set with this method or the constructor
     * @param status
     */
    void setStatus(RamStatus *status);

    RamState *state() const;
    int completionRatio() const;
    int version() const;
    QString comment() const;

signals:
    void statusUpdated(RamState*, int completion, int version, QString comment);

private slots:
    void currentStateChanged(RamState *state);
    void updateStatus();
    void adjustCommentEditSize();
    void revert();

private:
    void setupUi();
    void connectEvents();
    StateBox *ui_stateBox;
    DuQFSpinBox *ui_completionBox;
    AutoSelectSpinBox *ui_versionBox;
    QPlainTextEdit *ui_commentEdit;
    QToolButton *ui_setButton;
    QToolButton *ui_revertButton;

    RamStatus *m_status;
};

#endif // STATUSEDITWIDGET_H
