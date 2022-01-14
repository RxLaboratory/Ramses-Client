#include "ramworkingfolder.h"

#include "ramobject.h"
#include "ramnamemanager.h"

RamWorkingFolder::RamWorkingFolder(QString path)
{
    m_path = path;
    if (!m_path.endsWith("/") || !m_path.endsWith("\\")) m_path += "/";
}

const QString &RamWorkingFolder::path() const
{
    return m_path;
}

bool RamWorkingFolder::isPublished(int version, QString resource) const
{
    // List content
    QDir dir( publishPath() );
    QStringList subdirs = dir.entryList( QDir::Dirs | QDir::NoDotAndDotDot );
    for (int i = 0; i < subdirs.count(); i++)
    {
        QString dirname = subdirs.at(i);
        QStringList splitDirname = dirname.split("_");
        QString res = "";
        int v  = 0;
        if (splitDirname.count() == 3)
        {
            res = splitDirname.at(0);
            v = splitDirname.at(1).toInt();
        }
        else
        {
            v = splitDirname.at(0).toInt();
        }

        if (resource == res)
        {
            // Found the version
            if (version > 0 && version == v) return true;
            // Don't check the version
            if (version == 0) return true;
        }
    }
    return false;
}

QString RamWorkingFolder::defaultWorkingFile() const
{
    QStringList files = workingFiles();
    RamNameManager nm;
    foreach( QString file, files)
    {
        nm.setFileName(file);
        if (nm.resource() == "") return file;
    }
    return "";
}

QStringList RamWorkingFolder::workingFiles() const
{
    return listFiles(m_path);
}

QStringList RamWorkingFolder::publishedFiles(QString resource, int version) const
{
    // List content
    QString path = publishPath();
    QDir dir( path );
    QStringList subdirs = dir.entryList( QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name | QDir::IgnoreCase );
    QStringList publishedFiles;
    for (int i = subdirs.count() - 1; i >= 0; i--)
    {
        QString dirname = subdirs.at(i);
        QStringList splitDirname = dirname.split("_");
        int v = 0;
        QString res = "";
        if (splitDirname.count() == 3)
        {
            res = splitDirname.at(0);
            v = splitDirname.at(1).toInt();
        }
        else
        {
            v = splitDirname.at(0).toInt();
        }
        if (res == resource)
        {
            // Check version
            if (version == 0 || version == v)
            {
                // list files
                publishedFiles = listFiles( path + dirname );
                if (publishedFiles.count() > 0) return publishedFiles;
            }
        }
    }
    return publishedFiles;
}

QString RamWorkingFolder::publishPath() const
{
    QString p = m_path + RamObject::subFolderName(RamObject::PublishFolder) + "/";
    return p;
}

QString RamWorkingFolder::previewPath() const
{
    QString p = m_path + RamObject::subFolderName(RamObject::PreviewFolder) + "/";
    return p;
}

QString RamWorkingFolder::versionsPath() const
{
    QString p = m_path + RamObject::subFolderName(RamObject::VersionsFolder) + "/";
    return p;
}

QStringList RamWorkingFolder::listFiles(QString folderPath) const
{
    QDir dir(folderPath);
    QStringList files = dir.entryList( QDir::Files );

    QStringList result;
    for (int i = 0; i < files.count(); i++)
    {
        QString file = files.at(i);
        if (file == RamNameManager::MetaDataFileName) continue;

        result.append(m_path + file);
    }
    return result;
}

