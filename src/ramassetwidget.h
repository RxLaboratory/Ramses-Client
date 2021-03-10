#ifndef RAMASSETWIDGET_H
#define RAMASSETWIDGET_H

#include "ui_ramassetwidget.h"
#include "ramses.h"
#include "asseteditwidget.h"

#include <QDialog>
#include <QVBoxLayout>

class RamAssetWidget : public QWidget, private Ui::RamAssetWidget
{
    Q_OBJECT

public:
    explicit RamAssetWidget(RamAsset *asset, QWidget *parent = nullptr);
    RamAsset *asset() const;

private slots:
    void assetChanged();
    void userChanged();
    void edit();
private:
    RamAsset *_asset;
    QDialog *_editDialog;
};

#endif // RAMASSETWIDGET_H
