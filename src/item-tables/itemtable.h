#ifndef ITEMTABLE_H
#define ITEMTABLE_H

#include <QWidget>
#include <QShowEvent>
#include <QHideEvent>

#include "duqf-widgets/titlebar.h"
#include "duqf-utils/guiutils.h"
#include "itemtablewidget.h"


class ItemTable : public QWidget
{
    Q_OBJECT
public:
    explicit ItemTable(QString title= "Items Table", QWidget *parent = nullptr);

signals:
    void closeRequested();

protected:
    void showEvent(QShowEvent *event) Q_DECL_OVERRIDE;
    void hideEvent(QHideEvent *event) Q_DECL_OVERRIDE;

    ItemTableWidget *m_table;

protected slots:
    virtual void projectChanged(RamProject *project) = 0;

private:
    TitleBar *m_titleBar;

    void setupUi(QString title);
    void connectEvents();
};

#endif // ITEMTABLE_H
