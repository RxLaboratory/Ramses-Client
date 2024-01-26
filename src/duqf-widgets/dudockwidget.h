#ifndef DUDOCKWIDGET_H
#define DUDOCKWIDGET_H

#include <QDockWidget>

class DuDockWidget : public QDockWidget
{
    Q_OBJECT
public:
    DuDockWidget(QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());
    DuDockWidget(const QString &title, QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());

    void setWidget(QWidget *w); // Override to set css class
};

#endif // DUDOCKWIDGET_H
