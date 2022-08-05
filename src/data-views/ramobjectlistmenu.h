#ifndef RAMOBJECTLISTMENU_H
#define RAMOBJECTLISTMENU_H

#include <QMenu>
#include "data-models/ramobjectfiltermodel.h"

/**
 * @brief The RamObjectListMenu class is a QMenu showing a RamObjectList.
 * The items can be checkable to be used as a selection list, and in this case the menu contains a 'Select all' and a 'Select none' QAction.
 * A 'Create new...' QAction can also be added to the top of the list.
 */
template<typename RO> class RamObjectListMenu : public QMenu
{
    Q_OBJECT
public:
    RamObjectListMenu(bool checkable = false, QWidget *parent = nullptr);
    void setList(RamObjectList<RO> *list);
    void addCreateButton();
    RamObjectFilterModel<RO> *filteredList();

    void setObjectVisible(RO obj, bool visible = true);
    void showAll();

    bool isAllChecked() const;

public slots:
    void selectAll();
    void selectNone();
    void filter(RO o);
    void select(RO o);

    void saveState(QSettings *settings, QString group) const;
    void restoreState(QSettings *settings, QString group);

signals:
    void createTriggered();
    void assignmentChanged(RamObject*,bool);
    void assigned(RamObject*);

private slots:
    void newObject(const QModelIndex &parent, int first, int last);
    void removeObject(const QModelIndex &parent, int first, int last);
    void objectChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int>());

    void actionAssign(bool checked);
    void actionAssign();
    void actionCreate();

    void clear();
    void reset();

private:
    // UTILS //
    QString objectUuid(QAction *a);
    RO object(QAction *a);

    RamObjectFilterModel<RO> *m_objects;
    RamObjectList<RO> *m_emptyList;
    bool m_checkable = false;
};

#endif // RAMOBJECTLISTMENU_H
