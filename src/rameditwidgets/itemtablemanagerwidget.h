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
    TitleBar *ui_titleBar;
    RamObjectListComboBox *ui_groupBox;

protected slots:
    virtual void projectChanged(RamProject *project);

private slots:
    void addStep(const QModelIndex &parent, int first, int last);
    void removeStep(const QModelIndex &parent, int first, int last);
    void stepChanged(const QModelIndex &first, const QModelIndex &last,QVector<int> roles = QVector<int>());

    void addUser(const QModelIndex &parent, int first, int last);
    void removeUser(const QModelIndex &parent, int first, int last);
    void userChanged(const QModelIndex &first, const QModelIndex &last,QVector<int> roles = QVector<int>());

    void addState(const QModelIndex &parent, int first, int last);
    void removeState(const QModelIndex &parent, int first, int last);
    void stateChanged(const QModelIndex &first, const QModelIndex &last,QVector<int> roles = QVector<int>());

    void stepActionToggled(bool checked);
    void userActionToggled(bool checked);
    void stateActionToggled(bool checked);

    void checkStepFilters();
    void checkUserFilters();
    void checkStateFilters();

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

    // Item
    void createItem();
    void deleteItems();
    void createMultiple();

    void contextMenuRequested(QPoint p);

    void currentUserChanged(RamUser *user);

private:
    void setupUi();
    void connectEvents();

    DuQFSearchEdit *ui_searchEdit;
    QToolButton *ui_itemButton;
    QAction *ui_actionItem;
    QMenu *ui_itemMenu;
    QAction *ui_actionCreateItem;
    QAction *ui_actionDeleteItem;
    QAction *ui_actionCreateMultiple;
    QToolButton *ui_userButton;
    QMenu *ui_userMenu;
    QAction *ui_actionSelectAllUsers;
    QAction *ui_actionSelectNoUser;
    QAction *ui_actionSelectMyself;
    QAction *ui_actionNotAssigned;
    QToolButton *ui_stateButton;
    QMenu *ui_stateMenu;
    QAction *ui_actionSelectAllStates;
    QAction *ui_actionSelectNoState;
    QToolButton *ui_stepButton;
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
    QMenu *ui_contextMenu;
    RamObjectListMenu *ui_assignUserContextMenu;
    RamObjectListMenu *ui_changeStateContextMenu;

    RamProject *m_project = nullptr;
    RamStep::Type m_productionType;

    QList<QMetaObject::Connection> m_projectConnections;

    // utils
    QList<RamStatus*> beginEditSelectedStatus();
};

#endif // ITEMTABLE_H
