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
    enum EditMode { UnassignObjects, RemoveObjects };
    Q_ENUM( EditMode )

    explicit ObjectListEditWidget(bool editableObjects = false, QWidget *parent = nullptr);
    explicit ObjectListEditWidget(RamObjectList *objectList, bool editableObjects = false, QWidget *parent = nullptr);
    void setList(RamObjectList *objectList);
    void setFilterList(RamObjectList *filterList);
    void clear();
    void setEditMode(ObjectListEditWidget::EditMode editMode);
    void setEditable(bool editable = true);
    void setSearchable(bool searchable = true);
    void setTitle(QString title);
    void select(RamObject *o);
    QToolButton *addButton() const;
    QString currentFilterUuid() const;
    RamObject *currentFilter() const;

signals:
    void objectSelected(RamObject*);
    void add();
    void currentFilterChanged(QString);

private slots:
    void filterChanged(QString filter);
    void removeSelectedObjects();

private:
    void setupUi(bool editableObjects = false);
    void connectEvents();

    // UI Controls
    QToolButton *m_addButton;
    QToolButton *m_removeButton;
    QLabel *m_title;
    RamObjectListComboBox *m_filterBox;
    DuQFSearchEdit *m_searchEdit;
    RamObjectListWidget *m_listWidget;

    // Settings
    EditMode m_editMode = UnassignObjects;

    // Current List
    RamObjectList *m_objectList = nullptr;

    QMap<QString, QList<QMetaObject::Connection>> m_filterConnections;
};

#endif // OBJECTLISTEDITWIDGET_H
