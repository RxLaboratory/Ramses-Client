#ifndef OBJECTLISTEDITWIDGET_H
#define OBJECTLISTEDITWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QToolButton>
#include <QMessageBox>
#include <QScrollBar>
#include <QMenu>

#include "ramobjectlistwidget.h"
#include "ramobjectlistcombobox.h"
#include "duqf-widgets/duqfsearchedit.h"
#include "filetypeeditwidget.h"

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
    enum EditMode { UnassignObjects, RemoveObjects };
    Q_ENUM( EditMode )

    explicit ObjectListEditWidget(bool editableObjects = false, RamUser::UserRole editRole = RamUser::Admin, QWidget *parent = nullptr);
    explicit ObjectListEditWidget(RamObjectList *objectList, bool editableObjects = false, RamUser::UserRole editRole = RamUser::Admin, QWidget *parent = nullptr);
    void setList(RamObjectList *objectList);
    void setFilterList(RamObjectList *filterList);
    void setAssignList(RamObjectList *assignList);
    void clear();
    void setEditMode(ObjectListEditWidget::EditMode editMode);
    void setEditable(bool editable = true);
    void setSearchable(bool searchable = true);
    void setTitle(QString title);
    void select(RamObject *o);
    QToolButton *addButton() const;
    QString currentFilterUuid() const;
    RamObject *currentFilter() const;

public slots:
    void setFilter(RamObject *o);

signals:
    void objectSelected(RamObject*);
    void add();

private slots:
    void removeSelectedObjects();
    void edit(RamObject *obj);

    void newAssignObj(RamObject *obj);
    void newAssignObj(const QModelIndex &parent,int first,int last);
    void assignObjRemoved(const QModelIndex &parent,int first,int last);
    void assignObjChanged(RamObject *changedObj);

    void assignAction();

    void objectAssigned(const QModelIndex &parent,int first,int last);
    void objectUnassigned(const QModelIndex &parent,int first,int last);

private:
    void setupUi(bool editableObjects = false, RamUser::UserRole editRole = RamUser::Admin);
    void connectEvents();

    // UI Controls
    QToolButton *m_addButton;
    QToolButton *m_removeButton;
    QLabel *m_title;
    RamObjectListComboBox *m_filterBox;
    DuQFSearchEdit *m_searchEdit;
    RamObjectListWidget *m_listWidget;
    QMenu *m_assignMenu;

    // Settings
    EditMode m_editMode = UnassignObjects;
    bool m_useAssignList = false;

    // Current List
    RamObjectList *m_objectList = nullptr;
    RamObjectList *m_assignList = nullptr;
    RamObjectList *m_filterList = nullptr;

    QList<QMetaObject::Connection> m_listConnections;
};

#endif // OBJECTLISTEDITWIDGET_H
