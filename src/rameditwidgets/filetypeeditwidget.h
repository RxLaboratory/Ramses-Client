#ifndef FILETYPEEDITWIDGET_H
#define FILETYPEEDITWIDGET_H

#include "ui_filetypeeditwidget.h"
#include "ramses.h"

class FileTypeEditWidget : public QWidget, private Ui::FileTypeEditWidget
{
    Q_OBJECT

public:
    explicit FileTypeEditWidget(QWidget *parent = nullptr);

    RamFileType *fileType() const;
    void setFileType(RamFileType *fileType);

private slots:
    void update();
    void revert();
    bool checkInput();
    void fileTypeRemoved(RamObject *o);
    void dbiLog(DuQFLog m);

private:
    RamFileType *_fileType;
    QMetaObject::Connection _currentFileTypeConnection;
};

#endif // FILETYPEEDITWIDGET_H
