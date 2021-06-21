#ifndef FILETYPEEDITWIDGET_H
#define FILETYPEEDITWIDGET_H

#include <QCheckBox>

#include "ramses.h"
#include "objecteditwidget.h"

class FileTypeEditWidget : public ObjectEditWidget
{
    Q_OBJECT

public:
    explicit FileTypeEditWidget(RamFileType *fileType = nullptr);

    RamFileType *fileType() const;

public slots:
    void setObject(RamObject *obj) Q_DECL_OVERRIDE;

protected slots:
    void update() Q_DECL_OVERRIDE;

private slots:

private:
    RamFileType *_fileType;

    void setupUi();
    void connectEvents();

    QLineEdit *ui_extensionsEdit;
    QCheckBox *ui_previewableBox;
};

#endif // FILETYPEEDITWIDGET_H
