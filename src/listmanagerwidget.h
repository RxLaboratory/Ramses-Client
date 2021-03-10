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
    void addFilter(QString name, QString data);
    void updateFilterName(QString name, QString data);
    void clearFilters();
    void removeFilter(QString data);

signals:
    void filterChanged(QString);

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
    void filterIndexChanged(int i);
    void selectRow(int r);

private:
    RamUser::UserRole _role;
};

#endif // LISTMANAGERWIDGET_H
