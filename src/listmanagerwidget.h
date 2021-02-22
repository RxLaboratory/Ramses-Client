#ifndef LISTMANAGERWIDGET_H
#define LISTMANAGERWIDGET_H

#include <QtDebug>
#include <QMessageBox>

#include "ui_listmanagerwidget.h"
#include "ramses.h"

class ListManagerWidget : public QWidget, protected Ui::ListManagerWidget
{
    Q_OBJECT

public:
    explicit ListManagerWidget(QWidget *parent = nullptr);
    void setWidget(QWidget *w);

protected slots:
    void addItem(QListWidgetItem *item);
    void insertItem(int index, QListWidgetItem *item);
    void list_currentRowChanged(int currentRow);
    virtual void currentRowChanged(int currentRow);
    virtual void currentDataChanged(QVariant data);
    virtual void createItem();
    virtual void removeItem(QVariant data);
    void removeRow(int i);
    void removeData(QVariant data);
    void updateItem(QVariant data, QString text);
    void setRole(RamUser::UserRole r);

private slots:
    void remove_clicked();
    void loggedIn(RamUser *user);
    void loggedOut();

private:
    RamUser::UserRole _role;
};

#endif // LISTMANAGERWIDGET_H
