#ifndef STATUSEDITWIDGET_H
#define STATUSEDITWIDGET_H

#include <QPlainTextEdit>
#include <QToolButton>
#include <QFormLayout>
#include <QCheckBox>
#include <QListWidget>
#include <QShortcut>
#include <QMenu>
#include <QSplitter>

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

    RamStatus::Difficulty difficulty() const;

protected slots:
    void update() override;

protected:
    void showEvent(QShowEvent *event) override;

private slots:
    void currentStateChanged(RamObject *stateObj);
    void revert();
    void checkPublished(int v);

    void mainFileSelected(int row);
    void openMainFile();
    void removeSelectedMainFile();

    void createFromTemplate();

    void loadPublishedFiles();
    void publishedFileSelected(int row);
    void openPublishedFile();
    void removeSelectedPublishedFile();

    void previewFileSelected(int row);
    void openPreviewFile();
    void removeSelectedPreviewFile();

    void autoEstimate(bool useAutoEstimation);
    void autoEstimate();
    void estimateDays(int hours);

private:
    void setupUi();
    void connectEvents();
    StateBox *ui_stateBox;
    DuQFSpinBox *ui_completionBox;
    AutoSelectSpinBox *ui_versionBox;
    QPlainTextEdit *ui_statusCommentEdit;
    QToolButton *ui_revertButton;
    QCheckBox *ui_publishedBox;
    RamObjectListComboBox *ui_userBox;
    AutoSelectSpinBox *ui_timeSpent;
    QListWidget *ui_mainFileList;
    QListWidget *ui_previewFileList;
    QListWidget *ui_publishedFileList;
    QComboBox *ui_versionFileBox;
    QComboBox *ui_versionPublishBox;
    QToolButton *ui_openMainFileButton;
    QToolButton *ui_createMainFileButton;
    QMenu *ui_createFromTemplateMenu;
    QToolButton *ui_openPublishedFileButton;
    QToolButton *ui_openPreviewFileButton;
    DuQFFolderDisplayWidget *ui_folderWidget;
    QComboBox *ui_difficultyBox;
    AutoSelectDoubleSpinBox *ui_estimationEdit;
    QCheckBox *ui_autoEstimationBox;
    QLabel *ui_estimationLabel;

    QSplitter *ui_commentSplitter;

    RamStatus *m_status;
};

#endif // STATUSEDITWIDGET_H
