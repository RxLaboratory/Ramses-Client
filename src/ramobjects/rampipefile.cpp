#include "rampipefile.h"

#include "pipefileeditwidget.h"

// STATIC //

RamPipeFile *RamPipeFile::getObject(QString uuid, bool constructNew)
{
    RamObject *obj = RamObject::getObject(uuid);
    if (!obj && constructNew) return new RamPipeFile( uuid );
    return qobject_cast<RamPipeFile*>( obj );
}

// PUBLIC //

RamPipeFile::RamPipeFile(QString shortName, RamProject *project) :
    RamObject(shortName, shortName, PipeFile, project)
{
    construct();

    m_project = project;
}

RamFileType *RamPipeFile::fileType() const
{
    return RamFileType::getObject( getData("fileType").toString(), true );
}

void RamPipeFile::setFileType(RamFileType *newFileType)
{
    QJsonObject d = data();

    // set file type
    d.insert("fileType", newFileType->uuid());

    // Update the custom settings
    QString format;
    QStringList extensions = newFileType->extensions();
    if (extensions.count() > 0)
        format = newFileType->extensions()[0];
    else
        format = "*";

    QStringList settings = d.value("customSettings").toString().split("\n");
    bool found = false;

    for(int i = 0; i < settings.count(); i++)
    {
        QString s = settings[i].trimmed();
        if (s.startsWith("format:"))
        {
            found = true;
            QStringList setting = settings[i].split(":");
            if (setting.count() > 1) setting[1] = format;
            else setting << format;
            settings[i] = setting.join(": ");
            break;
        }
    }

    if (!found)
    {
        settings << "format: " + format;
    }

    d.insert("customSettings", settings.join("\n"));

    setData(d);
}

const RamProject *RamPipeFile::project() const
{
    return m_project;
}

QString RamPipeFile::customSettings() const
{
    return getData("customSettings").toString();
}

void RamPipeFile::setCustomSettings(const QString &newCustomSettings)
{
    insertData("customSettings", newCustomSettings);
}

QString RamPipeFile::details() const
{
    RamFileType *ft = fileType();
    if (ft) return "File type: " + ft->name();
    return RamObject::details();
}

void RamPipeFile::edit(bool show)
{
    if (!ui_editWidget) setEditWidget(new PipeFileEditWidget(this));

    if (show) showEdit();
}

// PROTECTED //

RamPipeFile::RamPipeFile(QString uuid):
    RamObject(uuid, PipeFile)
{
    construct();

    QJsonObject d = data();
    m_project = RamProject::getObject(d.value("project").toString(), true);

    this->setParent(m_project);
}

// PRIVATE //

void RamPipeFile::construct()
{
    m_icon = ":/icons/connection";
    m_editRole = ProjectAdmin;
}
