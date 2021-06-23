#ifndef ITEMTABLEMANAGERWIDGET_H
#define ITEMTABLEMANAGERWIDGET_H

#include <QWidget>
#include <QShowEvent>
#include <QHideEvent>
#include <QMenu>

#include "duqf-widgets/titlebar.h"
#include "duqf-utils/guiutils.h"
#include "duqf-widgets/duqfsearchedit.h"
#include "data-views/ramobjectlistwidget.h"
#include "ramses.h"


class ItemTableManagerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ItemTableManagerWidget(QString title= "Items Table", QWidget *parent = nullptr);

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

protected slots:
    virtual void projectChanged(RamProject *project) {Q_UNUSED(project)};

private slots:
    void stepAdded(RamStep *step);
    void stepRemoved(RamObject *stepObj);

    void stepActionToggled(bool checked);

private:
    void setupUi(QString title);
    void connectEvents();

    DuQFSearchEdit *ui_searchEdit;
    QMenu *ui_stepMenu;
    QAction *ui_actionSelectAllSteps ;
    QAction *ui_actionSelectNoSteps ;
    QAction *ui_actionSelectMySteps ;
};

#endif // ITEMTABLE_H
