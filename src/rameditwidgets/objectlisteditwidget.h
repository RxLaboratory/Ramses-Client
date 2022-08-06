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
#include <QShortcut>

#include "duqf-widgets/duqfsearchedit.h"
#include "ramobjectlistcombobox.h"
#include "ramobjectlistmenu.h"
#include "ramuser.h"
#include "ramobjectlistview.h"

/**
 * @brief The ObjectListEditWidget class displays and edits a RamObjectList
 * It does not handle object creation:
 * Connect to the "add" signal to create a new object in the list
 * The add QToolButton is available with addButton() to be able to add a submenu to it for example.
 * It includes some filters
 * And a searh field
 */
template<typename RO, typename ROF> class ObjectListEditWidget : public QWidget
{
    Q_OBJECT
public:
    enum EditMode { UnassignObjects, RemoveObjects };
    Q_ENUM( EditMode )

    explicit ObjectListEditWidget(bool editableObjects = false, RamUser::UserRole editRole = RamUser::Admin, QWidget *parent = nullptr);
    explicit ObjectListEditWidget(RamObjectList<RO> *objectList, bool editableObjects = false, RamUser::UserRole editRole = RamUser::Admin, QWidget *parent = nullptr);
    void setList(RamObjectList<RO> *objectList);
    void setFilterList(RamObjectList<ROF> *filterList);
    void setAssignList(RamObjectList<RO> *assignList);
    void setDontRemoveShortNameList(QStringList dontRemove);
    void clear();
    void setEditMode(ObjectListEditWidget::EditMode editMode);
    void setEditable(bool editable = true);
    void setSearchable(bool searchable = true);
    void setSortable(bool sortable = true);
    void setTitle(QString title);
    void select(RO o);
    QToolButton *addButton() const;
    QString currentFilterUuid() const;
    ROF currentFilter() const;

    RamObjectListView<RO> *listWidget();

public slots:
    void setFilter(ROF o);
    void removeSelectedObjects();

signals:
    void objectSelected(RO);
    void add();

private slots:
    void edit(RO obj);

    void assign(RO obj);

    void objectAssigned(const QModelIndex &parent,int first,int last);
    void objectUnassigned(const QModelIndex &parent,int first,int last);

    void setSearchFocus();

private:
    void setupUi(bool editableObjects = false, RamUser::UserRole editRole = RamUser::Admin);
    void connectEvents();

    // UI Controls
    QToolButton *ui_addButton;
    QToolButton *ui_removeButton;
    QLabel *ui_title;
    RamObjectListComboBox<ROF> *ui_filterBox;
    DuQFSearchEdit *ui_searchEdit;
    RamObjectListView<RO> *ui_listWidget;
    RamObjectListMenu<RO> *ui_assignMenu = nullptr;

    // Settings
    EditMode m_editMode = UnassignObjects;
    bool m_useAssignList = false;
    QStringList m_dontRemove;

    // Current List
    RamObjectList<RO> *m_objectList = nullptr;
    RamObjectList<RO> *m_assignList = nullptr;
    RamObjectList<ROF> *m_filterList = nullptr;

    QList<QMetaObject::Connection> m_listConnections;
};

#endif // OBJECTLISTEDITWIDGET_H
