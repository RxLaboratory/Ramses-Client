#ifndef OBJECTLISTEDITWIDGET_H
#define OBJECTLISTEDITWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QToolButton>
#include <QMessageBox>
#include <QScrollBar>

#include "ramobjectlistwidget.h"
#include "ramobjectuberlist.h"
#include "ramobjectlistcombobox.h"
#include "duqf-widgets/duqfsearchedit.h"

/**
 * @brief The ObjectListEditWidget class displays and edits a RamObjectList
 * It does not handle object creation:
 * Connect to the "add" signal to create a new object in the list
 * The add QToolButton is available with addButton() to be able to add a submenu to it for example.
 * It includes some filters used if the list is an uberlist, (if showSubObjects is true)
 * And a searh field
 */
class ObjectListEditWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ObjectListEditWidget(bool editableObjects = false, QWidget *parent = nullptr);
    explicit ObjectListEditWidget(RamObjectList *objectList, bool editableObjects = false, QWidget *parent = nullptr);
    explicit ObjectListEditWidget(RamObjectUberList *objectList, bool editableObjects = false, QWidget *parent = nullptr);
    void setEditMode(const RamObjectListWidget::EditMode &editMode);
    RamObjectList *list() const;
    void clear();
    void setList(RamObjectList *objectList);
    void setList(RamObjectUberList *objectList, bool useFilters = true);
    void setFilterList(RamObjectList *filterList);
    void setEditable(bool editable = true);
    void setSortable(bool sortable = true);
    void setSelectable(bool selectable = true);
    void setSearchable(bool searchable = true);
    void setTitle(QString title);
    QToolButton *addButton() const;
    void select(RamObject* obj);
    QString currentFilterUuid() const;
    RamObject *currentFilter() const;

public slots:
    void scrollToBottom();

signals:
    void objectSelected(RamObject*);
    void orderChanged();
    void add();
    void currentFilterChanged(QString);

private slots:
    void filterChanged(QString filter);

private:
    void setupUi(bool editableObjects = false);
    void connectEvents();
    QToolButton *m_addButton;
    QToolButton *m_removeButton;
    QLabel *m_title;
    RamObjectListComboBox *m_filterBox;
    DuQFSearchEdit *m_searchEdit;
    RamObjectList *m_objectList;
    RamObjectUberList *m_objectUberList;
    RamObjectListWidget *m_list;
    QMap<QString, QList<QMetaObject::Connection>> m_filterConnections;
    QList<QMetaObject::Connection> m_uberListConnections;
};

#endif // OBJECTLISTEDITWIDGET_H
