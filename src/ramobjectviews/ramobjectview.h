#ifndef RAMOBJECTVIEW_H
#define RAMOBJECTVIEW_H

#include <QHeaderView>
#include <QSortFilterProxyModel>

#include "duwidgets/dutableview.h"
#include "duwidgets/dumenu.h"
#include "ramobjectdelegate.h"
#include "ramobjectsortfilterproxymodel.h"
#include "ramuser.h"

/**
 * @brief The RamObjectView class is the base class displaying for all lists in Ramses, displaying RamObject
 * It displays a RamObjectList using RamObjectDelegate for the painging.
 * It used mainly in ObjectListEditWidget (in order to manage the list)
 */
class RamObjectView : public DuTableView
{
    Q_OBJECT
public:
    enum EditMode { UnassignObjects, RemoveObjects };
    Q_ENUM( EditMode )
    enum DisplayMode { List, Table };
    Q_ENUM( DisplayMode )

    explicit RamObjectView(DisplayMode mode = List, QWidget *parent = nullptr);
    // Settings
    void setEditableObjects(bool editableObjects, RamUser::UserRole editRole = RamUser::Admin);
    void setSortable(bool sortable = true);
    void setSortRole(RamObject::DataRole role);
    void sort();
    // Content
    void setObjectModel(QAbstractItemModel *model);
    QAbstractItemModel *objectModel() const;

    void setContextMenuDisabled(bool disabled);

signals:
    void objectSelected(RamObject*);

public slots:
    void search(QString s);
    void select(RamObject *o);
    void filter(RamObject *o);
    void setCompletionRatio(bool showCompletion);
    void showDetails(bool s);

    void copyUuid();
    void copyPath();
    void copyFileName();

protected:
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;
    virtual void showEvent(QShowEvent *event) override;

    // Model
    RamObjectSortFilterProxyModel *m_objectModel;

protected slots:
    // Moved
    void rowMoved( int logicalIndex, int oldVisualIndex, int newVisualIndex);

private slots:
    void select(const QModelIndex &index);
    void contextMenuRequested(QPoint p);

private:
    void setupUi();
    void connectEvents();

    DisplayMode m_displayMode;

    // Delegate
    RamObjectDelegate *m_delegate;

    // Filters
    QString m_currentFilterUuid;

    // Context menu
    DuMenu *ui_contextMenu;
    QAction *ui_actionCopyUuid;
    QAction *ui_actionCopyPath;
    QAction *ui_actionCopyFileName;

    // UI Events
    bool m_layout = false;
    QModelIndex m_clicking = QModelIndex();
    bool m_contextMenuDisabled = false;
};

#endif // RAMOBJECTVIEW_H
