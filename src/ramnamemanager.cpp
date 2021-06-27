#include "ramnamemanager.h"
#include "ramses.h"

RamNameManager::RamNameManager()
{
    init();

    // get state shortnames
    QStringList stateShortNames;
    RamObjectList *states = Ramses::instance()->states();
    for(int i = 0; i < states->count(); i++)
        stateShortNames << states->at(i)->shortName();

    // get name regex
    m_reName = RegExUtils::getRegEx("ramses-filename", "PUB|WIP|V", stateShortNames.join("|"));
    m_reName.setPatternOptions(QRegularExpression::CaseInsensitiveOption | QRegularExpression::MultilineOption);

}

QString RamNameManager::fileName() const
{
    return m_fileName;
}

bool RamNameManager::setFileName(const QString &name)
{
    init();

    m_fileName = name;
    QRegularExpressionMatch match = m_reName.match( name );

    if (!match.hasMatch()) return false;

    m_project = match.captured(1);
    m_ramType = match.captured(2);

    if (m_ramType == AssetType || m_ramType == ShotType)
    {
        m_shortName = match.captured(3);
        m_step = match.captured(4);
    }
    else
    {
        m_step = match.captured(3);
        m_shortName = match.captured(4);
    }

    m_resource = match.captured(5);
    m_state = match.captured(6);

    int v = match.captured(7).toInt();
    if (v >= 0)
        m_version = v;

    m_extension = match.captured(8);

    return true;
}

QString RamNameManager::project() const
{
    return m_project;
}

QString RamNameManager::type() const
{
    return m_ramType;
}

QString RamNameManager::shortName() const
{
    return m_shortName;
}

QString RamNameManager::step() const
{
    return m_step;
}

QString RamNameManager::state() const
{
    return m_state;
}

QString RamNameManager::resource() const
{
    return m_resource;
}

int RamNameManager::version() const
{
    return m_version;
}

void RamNameManager::init()
{
    m_project = "";
    m_ramType = "";
    m_shortName = "";
    m_step = "";
    m_resource = "";
    m_state = "";
    m_version = -1;
    m_extension = "";
}
