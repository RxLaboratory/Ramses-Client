#ifndef ITEMTABLEMANAGERWIDGET_H
#define ITEMTABLEMANAGERWIDGET_H

#include <QWidget>
#include <QShowEvent>
#include <QHideEvent>
#include <QMenu>
#include <QComboBox>

#include "duqf-widgets/titlebar.h"
#include "duqf-utils/guiutils.h"
#include "duqf-widgets/duqfsearchedit.h"
#include "data-views/ramobjectlistwidget.h"
#include "data-views/ramobjectlistcombobox.h"
#include "data-views/ramstepheaderview.h"
#include "data-views/ramobjectlistmenu.h"
#include "ramses.h"


class ItemTableManagerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ItemTableManagerWidget(RamStep::Type productionType, QWidget *parent = nullptr);

public slots:
    void selectAllSteps();
    void selectUserSteps();
    void deselectSteps();

signals:
    void closeRequested();

protected:
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;

    RamObjectListWidget *ui_table;
    TitleBar *ui_titleBar;
    RamObjectListComboBox *ui_groupBox;

protected slots:
    virtual void projectChanged(RamProject *project);

private slots:
    void addStep(const QModelIndex &parent, int first, int last);
    void removeStep(const QModelIndex &parent, int first, int last);
    void stepChanged(const QModelIndex &first, const QModelIndex &last,QVector<int> roles = QVector<int>());

    void stepActionToggled(bool checked);

    void editObject(RamObject *obj) const;
    void historyObject(RamObject *obj) const;

    // Status
    void unassignUser();
    void assignUser(RamObject *usrObj);
    void changeState(RamObject *sttObj);
    void setVeryEasy();
    void setEasy();
    void setMedium();
    void setHard();
    void setVeryHard();
    void setDiffculty(RamStatus::Difficulty difficulty);
    void setCompletion();

private:
    void setupUi();
    void connectEvents();

    DuQFSearchEdit *ui_searchEdit;
    QMenu *ui_stepMenu;
    QAction *ui_actionSelectAllSteps ;
    QAction *ui_actionSelectNoSteps ;
    QAction *ui_actionSelectMySteps ;
    QAction *ui_actionTimeTracking ;
    QAction *ui_actionCompletionRatio ;
    RamObjectListMenu *ui_assignUserMenu;
    RamObjectListMenu *ui_changeStateMenu;
    QMenu *ui_changeDifficultyMenu;
    QAction *ui_veryEasy;
    QAction *ui_easy;
    QAction *ui_medium;
    QAction *ui_hard;
    QAction *ui_veryHard;
    RamStepHeaderView *ui_header;
    QAction *ui_completion0;
    QAction *ui_completion10;
    QAction *ui_completion25;
    QAction *ui_completion50;
    QAction *ui_completion75;
    QAction *ui_completion90;
    QAction *ui_completion100;

    RamStepFilterModel *m_stepFilter;
    RamProject *m_project = nullptr;
    RamStep::Type m_productionType;

    // utils
    QList<RamStatus*> beginEditSelectedStatus();
};

#endif // ITEMTABLE_H
