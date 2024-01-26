#ifndef DUDOCKTITLEWIDGET_H
#define DUDOCKTITLEWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QToolButton>
#include <QDockWidget>
#include <QPointer>
#include <QMenu>

#include "duqf-widgets/dulabel.h"

class DuDockTitleWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DuDockTitleWidget(QString title, QWidget *parent = nullptr);

    void setTitle(QString title);
    void setIcon(QString icon);
    void setMenu(QMenu *menu);
    void addActionButton(QAction *action);
    void setActionButtons(const QVector<QAction*> &actions);

private slots:
    void closeDockWidget();

private:
    DuLabel *_titleLabel;
    QLabel *_iconLabel;
    QPointer<QMenu> _menu;
    QToolButton *_menuButton;
    QVector<QToolButton*> _actionButtons;

    QHBoxLayout *_layout;

};

#endif // DUDOCKTITLEWIDGET_H
