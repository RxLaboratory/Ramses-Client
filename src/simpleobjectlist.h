#ifndef SIMPLEOBJECTLIST_H
#define SIMPLEOBJECTLIST_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QToolButton>

#include "ramobjectwidget.h"
#include "ramassetwidget.h"
#include "ramshotwidget.h"
#include "duqf-widgets/duqflistwidget.h"

class SimpleObjectList : public QWidget
{
    Q_OBJECT
public:
    explicit SimpleObjectList(bool editableObjects = false, QWidget *parent = nullptr);
    void setSortable(bool sortable = true);
    void addObject(RamObject *obj, bool edit = false);
    void removeObject(RamObject *obj);
public slots:
    void removeSelectedObjects();
    void clear();
signals:
    void add();
    void objectRemoved(RamObject*);
    void objectSelected(RamObject*);
private slots:
    void currentItemChanged(QListWidgetItem *previous, QListWidgetItem *current);
    void selectionChanged();
private:
    void setupUi();
    void connectEvents();
    QToolButton *m_addButton;
    QToolButton *m_removeButton;
    QToolButton *m_upButton;
    QToolButton *m_downButton;
    QLabel *m_filterLabel;
    QComboBox *m_filterBox;
    DuQFListWidget *m_list;

    bool m_editableObjects;

};

#endif // SIMPLEOBJECTLIST_H
