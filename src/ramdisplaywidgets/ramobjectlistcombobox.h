#ifndef RAMOBJECTLISTCOMBOBOX_H
#define RAMOBJECTLISTCOMBOBOX_H

#include <QComboBox>

#include "ramobjectlist.h"

class RamObjectListComboBox : public QComboBox
{
    Q_OBJECT
public:
    RamObjectListComboBox(QWidget *parent = nullptr);
    RamObjectListComboBox(RamObjectList *list, QWidget *parent = nullptr);

    void setList(RamObjectList *list);

    RamObject *currentObject();
    QString currentUuid();
    void setObject(QString uuid);
    void setObject(RamObject *obj);

signals:
    void currentObjectChanged(RamObject*);

private slots:
    void newObject(RamObject *obj);
    void objectRemoved(RamObject *obj);
    void objectChanged(RamObject *obj);
    void currentObjectChanged(int i);
private:
    RamObjectList *m_list;

    void connectEvents();

    QList<QMetaObject::Connection> m_listConnections;
    QMap<QString, QMetaObject::Connection> m_objectConnections;
};

#endif // RAMOBJECTLISTCOMBOBOX_H
