#ifndef STATUSEDITWIDGET_H
#define STATUSEDITWIDGET_H

#include <QPlainTextEdit>
#include <QToolButton>
#include <QFormLayout>
#include <QCheckBox>
#include <QShortcut>
#include <QSplitter>

#include "duwidgets/ducombobox.h"
#include "duwidgets/duqftextedit.h"
#include "duwidgets/duqfspinbox.h"
#include "duwidgets/autoselectspinbox.h"
#include "duwidgets/autoselectdoublespinbox.h"
#include "duwidgets/duqffolderdisplaywidget.h"
#include "duwidgets/duqffilelist.h"
#include "objecteditwidget.h"
#include "qdatetimeedit.h"
#include "ramstatebox.h"
#include "ramstatus.h"

/**
 * @brief The StatusEditWidget class is used to edit a RamStatus.
 */
class StatusEditWidget : public ObjectEditWidget
{
    Q_OBJECT
public:
    StatusEditWidget(QWidget *parent = nullptr);
    StatusEditWidget(RamStatus *status, QWidget *parent = nullptr);

    RamStatus *status() const;

protected:
    virtual void reInit(RamObject *o) override;

private slots:
    void setState(RamState *state);
    void refresh();
    void setVersion(int v);
    void setCompletion(int c);
    void setComment();
    void assignUser(RamObject *u);
    void setPublished(bool p);
    void setAutoEstimation(bool a);
    void setEstimation(double e);
    void setDifficulty(int d);
    void setUseDueDate(bool u);
    void setDueDate();
    void setPriority(QVariant p);

    void mainFileSelected();
    void openMainFile();
    void removeSelectedMainFile();

    void createFromTemplate();

    void loadPublishedFiles();
    void publishedFileSelected();
    void openPublishedFile();
    void removeSelectedPublishedFile();

    void previewFileSelected();
    void openPreviewFile();
    void removeSelectedPreviewFile();

    void updatePriorityColor();

private:
    void setupUi();
    void connectEvents();

    RamStateBox *ui_stateBox;
    DuQFSpinBox *ui_completionBox;
    AutoSelectSpinBox *ui_versionBox;
    QDateEdit *ui_dueDateEdit;
    QCheckBox *ui_useDueDateBox;
    DuQFTextEdit *ui_statusCommentEdit;
    QToolButton *ui_revertButton;
    QCheckBox *ui_publishedBox;
    RamObjectComboBox *ui_userBox;
    DuQFFileList *ui_mainFileList;
    DuQFFileList *ui_previewFileList;
    DuQFFileList *ui_publishedFileList;
    DuComboBox *ui_versionFileBox;
    DuComboBox *ui_versionPublishBox;
    QToolButton *ui_openMainFileButton;
    QToolButton *ui_createMainFileButton;
    DuMenu *ui_createFromTemplateMenu;
    QToolButton *ui_openPublishedFileButton;
    QToolButton *ui_openPreviewFileButton;
    DuQFFolderDisplayWidget *ui_folderWidget;
    QComboBox *ui_difficultyBox;
    AutoSelectDoubleSpinBox *ui_estimationEdit;
    QCheckBox *ui_autoEstimationBox;
    QLabel *ui_estimationLabel;
    DuComboBox *ui_priorityBox;

    QSplitter *ui_commentSplitter;

    RamStatus *m_status;
};

#endif // STATUSEDITWIDGET_H
