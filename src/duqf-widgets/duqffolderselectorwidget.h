#ifndef DUQFFOLDERSELECTORWIDGET_H
#define DUQFFOLDERSELECTORWIDGET_H

#include "ui_duqffolderselectorwidget.h"

#include <QFileDialog>

#include "duqf-utils/utils.h"

class DuQFFolderSelectorWidget : public QWidget, private Ui::DuQFFolderSelectorWidget
{
    Q_OBJECT

public:
    explicit DuQFFolderSelectorWidget(QWidget *parent = nullptr);
    QString path();
    void setPath(QString p);
    void setPlaceHolderText(QString t);
    void setDialogTitle(QString t);
signals:
    void pathChanged(QString);
    void pathChanging(QString);
private slots:
    void browseButton_clicked();
    void exploreButton_clicked();
    void folderEdit_editingFinished();
private:
    QString _dialogTitle;
};

#endif // DUQFFOLDERSELECTORWIDGET_H
