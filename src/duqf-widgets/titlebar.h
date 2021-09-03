#ifndef TITLEBAR_H
#define TITLEBAR_H

#include <QToolBar>
#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QWidget>
#include <QtDebug>

#include "toolbarspacer.h"

class TitleBar : public QToolBar
{
    Q_OBJECT

public:
    explicit TitleBar(QString title, bool mini = false, QWidget *parent = nullptr);
    void setTitle(QString title);
    void showReinitButton(bool show);
    void showCloseButton(bool show);
    QAction *insertRight(QWidget *w);
    void insertRight(QAction *a);
    QAction *insertLeft(QWidget *w);
    void insertLeft(QAction *a);

signals:
    void reinitRequested();
    void closeRequested();

private:
    void setupUi(bool mini);

    ToolBarSpacer *spacer;
    QLabel *titleLabel;
    QToolButton *reinitButton;
    QToolButton *closeButton;
    QAction *reinitAction;
    QAction *leftAction;
    QAction *rightAction;
};

#endif // TITLEBAR_H
