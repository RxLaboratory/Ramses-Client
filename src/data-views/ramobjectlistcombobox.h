#ifndef RAMOBJECTLISTCOMBOBOX_H
#define RAMOBJECTLISTCOMBOBOX_H

#include <QComboBox>
#include <QSortFilterProxyModel>

#include "data-models/ramobjectlist.h"
#include "data-models/ramobjectfilterlist.h"

class RamObjectListComboBox : public QComboBox
{
    Q_OBJECT
public:
    RamObjectListComboBox(QWidget *parent = nullptr);
    RamObjectListComboBox(bool isFilterBox, QWidget *parent = nullptr);
    RamObjectListComboBox(RamObjectList *list, QWidget *parent = nullptr);

    void setList(RamObjectList *list);

    RamObject *currentObject();
    QString currentUuid();
    void setObject(QString uuid);
    void setObject(RamObject *obj);

signals:
    void currentObjectChanged(RamObject*);
    void currentObjectChanged(QString);

private slots:
    void currentObjectChanged(int i);

private:
    bool m_isFilterBox = false;

    void connectEvents();
};

#endif // RAMOBJECTLISTCOMBOBOX_H
