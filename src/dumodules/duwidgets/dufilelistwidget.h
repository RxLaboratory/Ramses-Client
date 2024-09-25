#ifndef DUFILELISTWIDGET_H
#define DUFILELISTWIDGET_H

#include <QWidget>

#include "duapp/app-config.h"
#include "duwidgets/dutreewidget.h"
#include "qfileinfo.h"

class DuFileListWidget : public DuTreeWidget
{
    Q_OBJECT
public:
    DuFileListWidget(QWidget *parent = nullptr);

    void setList(QFileInfoList fileList);
    void addFile(QFileInfo file);
    QFileInfo currentFile() const;
    QString currentFilePath() const;
    QString currentFileName() const;
    QFileInfo file(int index) const;
    QString filePath(int index) const;
    QString fileName(int index) const;

    bool showDate() const;
    void setShowDate(bool newShowDate);

private:
    bool m_showDate = true;
};

#endif // DUFILELISTWIDGET_H
