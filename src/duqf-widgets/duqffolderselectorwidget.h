#ifndef DUQFFOLDERSELECTORWIDGET_H
#define DUQFFOLDERSELECTORWIDGET_H

#include <QFileDialog>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QToolButton>

#include "duqf-utils/utils.h"

class DuQFFolderSelectorWidget : public QWidget
{
    Q_OBJECT

public:
    enum SelectorType { Folder, File };
    Q_ENUM(SelectorType)

    explicit DuQFFolderSelectorWidget(SelectorType type = Folder, QWidget *parent = nullptr);
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
    void setupUi();

    QLineEdit *folderEdit;
    QToolButton *browseButton;
    QToolButton *exploreButton;

    SelectorType _type;
};

#endif // DUQFFOLDERSELECTORWIDGET_H
