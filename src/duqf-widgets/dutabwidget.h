#ifndef DUTABWIDGET_H
#define DUTABWIDGET_H

#include <QTabWidget>

class DuTabWidget : public QTabWidget
{
    Q_OBJECT
public:
    DuTabWidget(QWidget *parent = nullptr);
    void setIconOnly(bool onlyIcon);
};

#endif // DUTABWIDGET_H
