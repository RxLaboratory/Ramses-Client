#ifndef RAMOBJECTWIDGET_H
#define RAMOBJECTWIDGET_H

#include <QWidget>
#include <QDockWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QToolButton>
#include <QMessageBox>
#include <QPlainTextEdit>

#include "ramses.h"
#include "objecteditwidget.h"
#include "objectdockwidget.h"
#include "data-models/ramobjectlist.h"

/**
 * @brief The RamObjectWidget class is the base class for all small widgets used to show objects anywhere in the UI.
 * It can display a small "edit" button whcih will show an editwidget in the MainWindow Dock
 */
class RamObjectWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RamObjectWidget(RamObject *obj, QWidget *parent = nullptr);
    ~RamObjectWidget();
    RamObject *ramObject() const;
    void setObject(RamObject *o);

    void setEditable(bool editable = true);
    void setRemovable(bool removable = true);

    void setUserEditRole(RamUser::UserRole role);
    RamObjectList *editUsers() const;

    void setTitle(QString t);
    void setIcon(QString i);
    void setColor(QColor color);

    ObjectDockWidget *dockEditWidget() const;

    bool selected() const;
    void setSelected(bool selected);
    void select();

    bool alwaysShowPrimaryContent() const;
    void setAlwaysShowPrimaryContent(bool newAlwaysShowPrimaryContent);
    void showExploreButton(bool s = true);

public slots:
    void edit(bool e = true);

protected:
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
    void hideEvent(QHideEvent *event) Q_DECL_OVERRIDE;
    void explore(QString path);

    void setEditWidget(ObjectEditWidget *w);
    int primaryContentHeight() const;
    void setPrimaryContentHeight(int newPrimaryContentHeight);
    QVBoxLayout *primaryContentLayout;
    QVBoxLayout *secondaryContentLayout;
    QHBoxLayout *buttonsLayout;
    QWidget *primaryContentWidget;
    QWidget *secondaryContentWidget;

protected slots:
    virtual void exploreClicked() {};
    virtual void userChanged();
    virtual void objectChanged();

private slots:
    void objectRemoved();
    void remove();
    void adjustCommentEditSize();

private:
    void setupUi();
    void connectEvents();
    QToolButton *editButton;
    QToolButton *removeButton;
    QLabel *title;
    QLabel *icon;
    QPlainTextEdit *commentEdit;
    QToolButton *exploreButton;
    int m_primaryContentHeight = 0;
    bool m_alwaysShowPrimaryContent = false;

    bool _selected = false;
    bool _editable = true;

    RamObjectList *_editUsers;

    bool _hasEditWidget = false;
    RamObject *m_object;
    ObjectDockWidget *_dockEditWidget;
    RamUser::UserRole _editRole = RamUser::Admin;
    bool _hasEditRights = false;
    QColor _color;

    QList<QMetaObject::Connection> m_objectConnections;
};

#endif // RAMOBJECTWIDGET_H
