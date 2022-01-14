#ifndef ITEMTABLEMANAGERWIDGET_H
#define ITEMTABLEMANAGERWIDGET_H

#include <QWidget>
#include <QShowEvent>
#include <QHideEvent>
#include <QMenu>
#include <QComboBox>
#include <QClipboard>

#include "duqf-widgets/duqftitlebar.h"
#include "duqf-utils/guiutils.h"
#include "duqf-widgets/duqfsearchedit.h"
#include "data-views/ramobjectlistwidget.h"
#include "data-views/ramobjectlistcombobox.h"
#include "data-views/ramstepheaderview.h"
#include "data-views/ramobjectlistmenu.h"
#include "ramses.h"
#include "shotscreationdialog.h"

class ItemTableManagerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ItemTableManagerWidget(RamStep::Type productionType, QWidget *parent = nullptr);

public slots:
    void selectAllSteps();
    void selectUserSteps();
    void deselectSteps();

    void selectAllUsers();
    void selectMyself();
    void deselectUsers();
    void showUnassigned(bool show);

    void selectAllStates();
    void deselectStates();

signals:
    void closeRequested();

protected:
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;

    RamObjectListWidget *ui_table;
    DuQFTitleBar *ui_titleBar;
    RamObjectListComboBox *ui_groupBox;

protected slots:
    virtual void projectChanged(RamProject *project);

private slots:
    void showUser(RamObject *userObj, bool s);
    void showStep(RamObject *stepObj, bool s);
    void showState(RamObject *stateObj, bool s);

    void checkStepFilters();
    void checkUserFilters();
    void checkStateFilters();

    void editObject(RamObject *obj) const;
    void historyObject(RamObject *obj) const;

    // Sort
    void uncheckSort();
    void sortDefault(bool sort = true);
    void sortByShortName(bool sort = true);
    void sortByName(bool sort = true);
    void sortByDifficulty(bool sort = true);
    void sortByTimeSpent(bool sort = true);
    void sortByEstimation(bool sort = true);
    void sortByCompletion(bool sort = true);

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
    void copyComment();
    void cutComment();
    void pasteComment();

    // Item
    void createItem();
    void deleteItems();
    void createMultiple();

    void contextMenuRequested(QPoint p);

    void currentUserChanged(RamUser *user);

private:
    void setupUi();
    void connectEvents();
    void loadSettings();

    DuQFSearchEdit *ui_searchEdit;
    QToolButton *ui_itemButton;
    QAction *ui_actionItem;
    QMenu *ui_itemMenu;
    QAction *ui_actionCreateItem;
    QAction *ui_actionDeleteItem;
    QAction *ui_actionCreateMultiple;
    QToolButton *ui_userButton;
    RamObjectListMenu *ui_userMenu;
    QAction *ui_actionSelectMyself;
    QAction *ui_actionNotAssigned;
    QToolButton *ui_stateButton;
    RamObjectListMenu *ui_stateMenu;
    QToolButton *ui_stepButton;
    RamObjectListMenu *ui_stepMenu;
    QAction *ui_actionSelectMySteps ;
    QAction *ui_actionTimeTracking ;
    QAction *ui_actionCompletionRatio ;
    QAction *ui_actionShowDetails;
    QAction *ui_actionSortDefault;
    QAction *ui_actionSortByShortName;
    QAction *ui_actionSortByName;
    QAction *ui_actionSortByDifficulty;
    QAction *ui_actionSortByTimeSpent;
    QAction *ui_actionSortByEstimation;
    QAction *ui_actionSortByCompletion;
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
    QMenu *ui_contextMenu;
    QAction *ui_copyComment;
    QAction *ui_cutComment;
    QAction *ui_pasteComment;
    RamObjectListMenu *ui_assignUserContextMenu;
    RamObjectListMenu *ui_changeStateContextMenu;

    RamProject *m_project = nullptr;
    RamStep::Type m_productionType;

    QList<QMetaObject::Connection> m_projectConnections;

    // utils
    QList<RamStatus*> beginEditSelectedStatus();
};

#endif // ITEMTABLE_H
