#ifndef RAMFILETYPEWIDGET_H
#define RAMFILETYPEWIDGET_H

#include "ramobjectwidget.h"
#include "filetypeeditwidget.h"

class RamFileTypeWidget : public RamObjectWidget
{
    Q_OBJECT
public:
    RamFileTypeWidget(RamFileType *fileType, QWidget *parent = nullptr);
    RamFileType *fileType() const;
    void setFileType(RamFileType *fileType);

private:
    RamFileType *m_fileType;
};

#endif // RAMFILETYPEWIDGET_H
