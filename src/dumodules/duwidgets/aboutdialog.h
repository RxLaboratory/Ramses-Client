#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include "ui_aboutdialog.h"

#include "duwidgets/dudialog.h"

class AboutDialog : public DuDialog, private Ui::AboutDialog
{
public:
    explicit AboutDialog(QWidget *parent = nullptr);
};

#endif // ABOUTDIALOG_H
