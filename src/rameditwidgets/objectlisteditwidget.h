#ifndef OBJECTLISTEDITWIDGET_H
#define OBJECTLISTEDITWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QToolButton>
#include <QMessageBox>
#include <QScrollBar>

#include "ramobjectlistwidget.h"
#include "duqf-widgets/duqfsearchedit.h"

/**
 * @brief The ObjectListEditWidget class displays and edits a RamObjectList
 * It does not handle object creation:
 * Connect to the "add" signal to create a new object in the list
 * The add QToolButton is available with addButton() to be able to add a submenu to it for example.
 */
class ObjectListEditWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ObjectListEditWidget(bool editableObjects = false, QWidget *parent = nullptr);
    explicit ObjectListEditWidget(RamObjectList *objectList, bool editableObjects = false, QWidget *parent = nullptr);
    RamObjectList *list() const;
    void setList(RamObjectList *objectList);
    void setEditable(bool editable = true);
    void setSortable(bool sortable = true);
    void setSelectable(bool selectable = true);
    void setTitle(QString title);
    QToolButton *addButton() const;
    void select(RamObject* obj);
    QString currentFilter() const;

public slots:
    void scrollToBottom();

signals:
    void objectSelected(RamObject*);
    void orderChanged();
    void add();

private:
    void setupUi(bool editableObjects = false);
    void connectEvents();
    QToolButton *m_addButton;
    QToolButton *m_removeButton;
    QLabel *m_filterLabel;
    QLabel *m_title;
    QComboBox *m_filterBox;
    DuQFSearchEdit *m_searchEdit;
    RamObjectList *m_objectList;
    RamObjectListWidget *m_list;

};

#endif // OBJECTLISTEDITWIDGET_H
