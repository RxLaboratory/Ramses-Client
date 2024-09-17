#ifndef DUDOCKWIDGET_H
#define DUDOCKWIDGET_H

#include "duwidgets/dudocktitlewidget.h"
#include <QDockWidget>

class DuDockWidget : public QDockWidget
{
    Q_OBJECT
public:
    DuDockWidget(QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());
    DuDockWidget(const QString &title, QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());
    DuDockWidget(const QString &title, const QString &icon, QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());

    DuDockTitleWidget *dockTitleWidget() { return ui_dockTitleWidget; };

    void setWidget(QWidget *w); // Override to set css class
    void setAutoDeleteWidget(bool autoDelete = true);

    void deleteWidget();

protected:
    void closeEvent(QCloseEvent *e) override;

private:
    DuDockTitleWidget *ui_dockTitleWidget = nullptr;
    bool _autoDeleteWidget = false;
};

#endif // DUDOCKWIDGET_H
