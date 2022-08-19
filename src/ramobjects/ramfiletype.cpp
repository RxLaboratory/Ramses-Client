#include "ramfiletype.h"

#include "filetypeeditwidget.h"

// STATIC //

QMap<QString, RamFileType*> RamFileType::m_existingObjects = QMap<QString, RamFileType*>();

RamFileType *RamFileType::get(QString uuid)
{
    if (!checkUuid(uuid, FileType)) return nullptr;

    if (m_existingObjects.contains(uuid)) return m_existingObjects.value(uuid);

    // Finally return a new instance
    return new RamFileType(uuid);
}

RamFileType *RamFileType::c(RamObject *o)
{
    return qobject_cast<RamFileType*>(o);
}

RamFileType::RamFileType(QString shortName, QString name):
    RamObject(shortName, name, FileType)
{
    construct();
}

RamFileType::RamFileType(QString uuid):
    RamObject(uuid, FileType)
{
    construct();
}

void RamFileType::setExtensions(QString extensions)
{
    QStringList exts = extensions.split(",");
    setExtensions(exts);
}

void RamFileType::setExtensions(QStringList extensions)
{
    QJsonArray arr;
    for(int i = 0; i < extensions.count(); i++)
    {
        QString ext = extensions[i];
        if (ext.startsWith(".")) ext = ext.remove(0, 1);

        arr.append( ext.trimmed().toLower() );
    }
    insertData("extensions", arr);
}

QStringList RamFileType::extensions() const
{
    QJsonArray arr = getData("extensions").toArray();
    QStringList exts;
    for (int i = 0; i < arr.count(); i++)
    {
        exts << arr.at(i).toString();
    }
    return exts;
}

bool RamFileType::previewable() const
{
    return getData("previewable").toBool(false);
}

void RamFileType::setPreviewable(bool previewable)
{
    insertData("previewable", previewable);
}

bool RamFileType::check(QString filePath) const
{
    QFileInfo info(filePath);
    if (!info.isFile()) return false;
    QString ext = info.completeSuffix().toLower();

    if (shortName() == ext) return true;

    return extensions().contains(ext);
}

QString RamFileType::details() const
{
    return "Extensions: " + extensions().join(", ");
}

void RamFileType::edit(bool show)
{
    if (!ui_editWidget) setEditWidget(new FileTypeEditWidget(this));

    if (show) showEdit();
}

// PRIVATE //

void RamFileType::construct()
{
    m_existingObjects[m_uuid] = this;
    m_icon = ":/icons/file";
    m_editRole = Admin;
}





