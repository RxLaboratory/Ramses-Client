#ifndef DUQFFOLDERDISPLAYWIDGET_H
#define DUQFFOLDERDISPLAYWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QToolButton>
#include <QHBoxLayout>
#include <QMessageBox>

#include "duqf-utils/utils.h"

class DuQFFolderDisplayWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DuQFFolderDisplayWidget(QWidget *parent = nullptr);

public slots:
    void setPath(QString p);
signals:


private slots:
    void exploreButton_clicked();

private:
    void setupUi();
    QLabel *folderLabel;
    QToolButton *exploreButton;
};

#endif // DUQFFOLDERDISPLAYWIDGET_H
