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

private:
    void setupUi();
    void connectEvents();

    DuQFSearchEdit *ui_searchEdit;
    QMenu *ui_stepMenu;
    QAction *ui_actionSelectAllSteps ;
    QAction *ui_actionSelectNoSteps ;
    QAction *ui_actionSelectMySteps ;

    RamStepFilterModel *m_stepFilter;
    RamProject *m_project = nullptr;
    RamStep::Type m_productionType;
};

#endif // ITEMTABLE_H
