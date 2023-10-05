#ifndef TOOLBARSPACER_H
#define TOOLBARSPACER_H

#include <QWidget>
#include <QHBoxLayout>
#include <QSpacerItem>

class DuToolBarSpacer : public QWidget
{
public:
    explicit DuToolBarSpacer(QWidget *parent = nullptr);

private:
    void setupUi();
};


#endif // TOOLBARSPACER_H
