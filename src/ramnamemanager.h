#ifndef RAMNAMEMANAGER_H
#define RAMNAMEMANAGER_H

#include "duqf-utils/utils.h"

class RamNameManager
{
public:
    RamNameManager();

    QString fileName() const;
    bool setFileName(const QString &name);

    // Get results
    QString project() const;
    QString type() const;
    QString shortName() const;
    QString step() const;
    QString state() const;
    QString resource() const;
    int version() const;

private:
    void init();

    // Regexes
    QRegularExpression m_reName;

    // Utils - Constants
    QString ShotType = "S";
    QString AssetType = "A";
    QString GeneralType = "G";

    QString m_fileName;

    // Results
    QString m_project;
    QString m_ramType;
    QString m_shortName;
    QString m_step;
    QString m_resource;
    QString m_state;
    int m_version;
    QString m_extension;
};

#endif // RAMNAMEMANAGER_H
