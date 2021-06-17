#ifndef ITEMTABLE_H
#define ITEMTABLE_H

#include <QWidget>
#include <QShowEvent>
#include <QHideEvent>
#include <QMenu>

#include "duqf-widgets/titlebar.h"
#include "duqf-utils/guiutils.h"
#include "duqf-widgets/duqfsearchedit.h"
#include "itemtablewidget.h"


class ItemTable : public QWidget
{
    Q_OBJECT
public:
    explicit ItemTable(QString title= "Items Table", QWidget *parent = nullptr);

public slots:
    void selectAllSteps();
    void selectUserSteps();
    void deselectSteps();

signals:
    void closeRequested();

protected:
    void showEvent(QShowEvent *event) Q_DECL_OVERRIDE;
    void hideEvent(QHideEvent *event) Q_DECL_OVERRIDE;

    ItemTableWidget *m_table;
    TitleBar *m_titleBar;

protected slots:
    virtual void projectChanged(RamProject *project) {Q_UNUSED(project)};

private slots:
    void stepAdded(RamStep *step);
    void stepRemoved(RamObject *stepObj);

    void stepActionToggled(bool checked);

private:
    void setupUi(QString title);
    void connectEvents();

    DuQFSearchEdit *m_searchEdit;
    QMenu *m_stepMenu;
    QAction *m_actionSelectAllSteps ;
    QAction *m_actionSelectNoSteps ;
    QAction *m_actionSelectMySteps ;
};

#endif // ITEMTABLE_H
