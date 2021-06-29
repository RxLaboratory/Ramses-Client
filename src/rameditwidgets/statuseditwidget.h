#ifndef STATUSEDITWIDGET_H
#define STATUSEDITWIDGET_H

#include <QPlainTextEdit>
#include <QToolButton>
#include <QFormLayout>
#include <QCheckBox>
#include <QListWidget>
#include <QShortcut>
#include <QMenu>

#include "duqf-widgets/duqfspinbox.h"
#include "duqf-widgets/autoselectspinbox.h"
#include "duqf-widgets/autoselectdoublespinbox.h"
#include "data-views/ramobjectlistcombobox.h"
#include "duqf-widgets/duqffolderdisplaywidget.h"
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
    RamUser *assignedUser() const;
    bool isPublished() const;
    qint64 timeSpent() const;
    float estimation() const;
    RamStatus::Difficulty difficulty() const;

signals:
    void statusUpdated(RamState*, int completion, int version, QString comment);

private slots:
    void currentStateChanged(RamObject *stateObj);
    void updateStatus();
    void adjustCommentEditSize();
    void revert();
    void checkPublished(int v);

    void mainFileSelected(int row);
    void openMainFile();
    void removeSelectedMainFile();

    void createFromTemplate();
    void createFromDefaultTemplate();

    void publishedFileSelected(int row);
    void openPublishedFile();
    void removeSelectedPublishedFile();

    void previewFileSelected(int row);
    void openPreviewFile();
    void removeSelectedPreviewFile();

    void autoEstimate(bool estimate);
    void autoEstimate();
    void estimateDays(int hours);

private:
    void setupUi();
    void connectEvents();
    StateBox *ui_stateBox;
    DuQFSpinBox *ui_completionBox;
    AutoSelectSpinBox *ui_versionBox;
    QPlainTextEdit *ui_statusCommentEdit;
    QToolButton *ui_setButton;
    QToolButton *ui_revertButton;
    QCheckBox *ui_publishedBox;
    RamObjectListComboBox *ui_userBox;
    AutoSelectSpinBox *ui_timeSpent;
    QListWidget *ui_mainFileList;
    QListWidget *ui_previewFileList;
    QListWidget *ui_publishedFileList;
    QComboBox *ui_versionFileBox;
    QToolButton *ui_openMainFileButton;
    QToolButton *ui_createMainFileButton;
    QMenu *ui_createFromTemplateMenu;
    QAction *ui_createFromTemplateAction;
    QToolButton *ui_openPublishedFileButton;
    QToolButton *ui_openPreviewFileButton;
    DuQFFolderDisplayWidget *ui_folderWidget;
    QComboBox *ui_difficultyBox;
    AutoSelectDoubleSpinBox *ui_estimationEdit;
    QCheckBox *ui_autoEstimationBox;

    RamStatus *m_status;
};

#endif // STATUSEDITWIDGET_H
