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

#include "duwidgets/duaction.h"
#include "dutoolbarspacer.h"

class DuQFTitleBar : public QToolBar
{
    Q_OBJECT

public:
    explicit DuQFTitleBar(QString title, bool mini = false, QWidget *parent = nullptr);
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

private slots:
    void changeOrientation(Qt::Orientation orientation);

private:
    void setupActions(const QString &title);
    void setupUi(bool mini);

    DuAction *m_closeAction;
    
    DuToolBarSpacer *spacer;
    QLabel *titleLabel;
    QToolButton *reinitButton;
    QAction *reinitAction;
    QAction *leftAction;
    QAction *rightAction;

    QVector<QAction*> m_actions;
};

#endif // TITLEBAR_H
