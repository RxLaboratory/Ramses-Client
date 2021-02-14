#ifndef LISTMANAGERWIDGET_H
#define LISTMANAGERWIDGET_H

#include <QtDebug>

#include "ui_listmanagerwidget.h"

class ListManagerWidget : public QWidget, private Ui::ListManagerWidget
{
    Q_OBJECT

public:
    explicit ListManagerWidget(QWidget *parent = nullptr);
    void setWidget(QWidget *w);

protected slots:
    void addItem(QListWidgetItem *item);
    void list_currentRowChanged(int currentRow);
    virtual void currentRowChanged(int currentRow);
    virtual void currentDataChanged(QVariant data);
    virtual void createItem();
    virtual void removeItem(QVariant data);
    void removeRow(int i);
    void removeData(QVariant data);
    void updateItem(QVariant data, QString text);

private slots:
    void remove_clicked();
};

#endif // LISTMANAGERWIDGET_H
