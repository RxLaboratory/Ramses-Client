#ifndef OBJECTLISTWIDGET_H
#define OBJECTLISTWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QToolButton>
#include <QMessageBox>
#include <QShortcut>

#include "duwidgets/duqfsearchedit.h"
#include "ramobjectcombobox.h"
#include "ramobjectmenu.h"
#include "ramuser.h"
#include "ramobjectview.h"

/**
 * @brief The ObjectListWidget class displays and edits a RamObjectList
 * It does not handle object creation:
 * Connect to the "add" signal to create a new object in the list
 * The add QToolButton is available with addButton() to be able to add a submenu to it for example.
 * It includes some filters
 * And a searh field
 */
class ObjectListWidget : public QWidget
{
    Q_OBJECT
public:
    enum EditMode { NoEdit, UnassignObjects, RemoveObjects };
    Q_ENUM( EditMode )

    explicit ObjectListWidget(bool editableObjects = false, RamUser::UserRole editRole = RamUser::Admin, QWidget *parent = nullptr);
    explicit ObjectListWidget(QAbstractItemModel *objectList, bool editableObjects = false, RamUser::UserRole editRole = RamUser::Admin, QWidget *parent = nullptr);
    void setObjectModel(QAbstractItemModel *objectModel);
    void setFilterList(QAbstractItemModel *filterList, QString filterListName = "");
    void setAssignList(QAbstractItemModel *assignList);
    void setDontRemoveShortNameList(QStringList dontRemove);
    void clear();
    void setEditMode(ObjectListWidget::EditMode editMode);
    void setEditable(bool editable = true);
    void setSearchable(bool searchable = true);
    void setSortable(bool sortable = true);
    void setTitle(QString title);
    void select(RamObject *o);
    QToolButton *addButton() const;
    QString currentFilterUuid() const;
    RamObject *currentFilter() const;
    void setSortMode(RamObject::DataRole mode);
    void sort();

    RamObjectView *listWidget();

public slots:
    void setFilter(RamObject *o);
    void removeSelectedObjects();

signals:
    void objectSelected(RamObject*);
    void objectAssigned(RamObject*);
    void objectsUnassigned(QVector<RamObject*>);
    void add();

private slots:
    void assign(RamObject *obj);

    void assignObject(const QModelIndex &parent,int first,int last);
    void unassignObject(const QModelIndex &parent,int first,int last);

    void setSearchFocus();

private:
    void setupUi(bool editableObjects = false, RamUser::UserRole editRole = RamUser::Admin);
    void connectEvents();

    // UI Controls
    QToolButton *ui_addButton;
    QToolButton *ui_removeButton;
    QLabel *ui_title;
    RamObjectComboBox *ui_filterBox;
    DuQFSearchEdit *ui_searchEdit;
    RamObjectView *ui_objectView;
    RamObjectMenu *ui_assignMenu = nullptr;

    // Settings
    EditMode m_editMode = UnassignObjects;
    bool m_useAssignList = false;
    QStringList m_dontRemove;

    QList<QMetaObject::Connection> m_listConnections;
};

#endif // OBJECTLISTWIDGET_H
