#ifndef DUDIALOG_H
#define DUDIALOG_H

#include "duwidgets/duiconwidget.h"
#include <QDialog>
#include <QToolBar>
#include <QBoxLayout>
#include <QLabel>
#include <QToolButton>

class DuDialog : public QDialog
{
    Q_OBJECT
public:
    DuDialog(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

    void setWindowTitle(const QString &title);

    QBoxLayout *layout() { return ui_layout; }
    void setLayout(QLayout *l) { ui_layout->addLayout(l); }

    QToolBar *titleBar() const { return ui_titleBar; }

public slots:
    virtual int exec() override;

protected:
    virtual bool eventFilter(QObject *obj, QEvent *event) override;
    void paintEvent(QPaintEvent *ev) override;

private:
    QBoxLayout *ui_layout;
    QToolBar *ui_titleBar;
    QLabel *ui_titleLabel;
    QToolButton *ui_closeButton;

    // Settings
    int _margin;

    // Interactions
    bool _launching = false;
    bool _toolbarClicked = false;
    QPoint _dragPos;
};

#endif // DUDIALOG_H
