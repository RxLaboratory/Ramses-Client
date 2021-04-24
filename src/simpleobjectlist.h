#ifndef SIMPLEOBJECTLIST_H
#define SIMPLEOBJECTLIST_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QToolButton>
#include <QMessageBox>
#include <QScrollBar>

#include "ramobjectwidget.h"
#include "ramassetwidget.h"
#include "ramshotwidget.h"
#include "ramstatuswidget.h"
#include "duqf-widgets/duqflistwidget.h"
#include "objectlistwidget.h"

class SimpleObjectList : public QWidget
{
    Q_OBJECT
public:
    explicit SimpleObjectList(bool editableObjects = false, QWidget *parent = nullptr);
    void setEditable(bool editable = true);
    void setSortable(bool sortable = true);
    void setTitle(QString title);
    void addObject(RamObject *obj, bool edit = false);
    QList<RamObject*> ramObjects() const;
    QToolButton *addButton() const;
public slots:
    void removeObject(RamObject *obj);
    void removeObject(QString uuid);
    void clear();
    void scrollToBottom();
signals:
    void add();
    void objectRemoved(RamObject*);
    void objectSelected(RamObject*);
    void orderChanged();
private:
    void setupUi();
    void connectEvents();
    QToolButton *m_addButton;
    QToolButton *m_removeButton;
    QLabel *m_filterLabel;
    QLabel *m_title;
    QComboBox *m_filterBox;
    ObjectListWidget *m_list;

    bool m_editableObjects;

};

#endif // SIMPLEOBJECTLIST_H
