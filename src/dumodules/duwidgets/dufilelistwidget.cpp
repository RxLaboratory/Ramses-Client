#include "dufilelistwidget.h"

#include <QDateTime>
#include <QHeaderView>

#include "duapp/dusettings.h"
#include "duwidgets/duicon.h"

DuFileListWidget::DuFileListWidget(QWidget *parent):
    DuTreeWidget(parent)
{
    this->setColumnCount(2);
    this->setHeaderLabels(QStringList() << "Name" << "Date");
}

void DuFileListWidget::setList(QFileInfoList fileList)
{
    this->clear();
    foreach(QFileInfo file, fileList)
    {
        addFile(file);
    }
    this->header()->resizeSections(QHeaderView::ResizeToContents);
}

void DuFileListWidget::addFile(QFileInfo file)
{
    // TODO if folder: show as parent with files inside
    QStringList label;
    label << file.fileName() << file.lastModified().toString(
        DuSettings::i()->get(DuSettings::UI_DateTimeFormat).toString()
        );
    QTreeWidgetItem *item = new QTreeWidgetItem(label);
    item->setData(0, Qt::UserRole, file.absoluteFilePath() );
    item->setData(0, Qt::UserRole + 1, file.fileName() );
    if (file.isSymLink()) item->setIcon(0, DuIcon(":/icons/shortcut"));
    else if (file.isFile()) item->setIcon(0, DuIcon(":icons/file"));
    else if (file.isDir()) item->setIcon(0, DuIcon(":icons/folder"));
    this->addTopLevelItem(item);
}

QFileInfo DuFileListWidget::currentFile() const
{
    return QFileInfo( currentFilePath() );
}

QString DuFileListWidget::currentFilePath() const
{
    return this->currentItem()->data(0, Qt::UserRole).toString();
}

QString DuFileListWidget::currentFileName() const
{
    return this->currentItem()->data(0, Qt::UserRole + 1).toString();
}

QFileInfo DuFileListWidget::file(int index) const
{
    return QFileInfo(filePath(index));
}

QString DuFileListWidget::filePath(int index) const
{
    return this->topLevelItem(index)->data(0, Qt::UserRole).toString();
}

QString DuFileListWidget::fileName(int index) const
{
    return this->topLevelItem(index)->data(0, Qt::UserRole + 1).toString();
}

bool DuFileListWidget::showDate() const
{
    return m_showDate;
}

void DuFileListWidget::setShowDate(bool newShowDate)
{
    m_showDate = newShowDate;
}
