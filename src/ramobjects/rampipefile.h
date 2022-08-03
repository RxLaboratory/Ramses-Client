#ifndef RAMPIPEFILE_H
#define RAMPIPEFILE_H

#include <QObject>

#include "ramobject.h"

class RamProject;
class RamFileType;

class RamPipeFile : public RamObject
{
    Q_OBJECT
public:

    // STATIC //

    static RamPipeFile *getObject(QString uuid, bool constructNew = false);

    // OTHER //

    RamPipeFile(QString shortName, RamProject *project);

    RamFileType *fileType() const;
    void setFileType(RamFileType *newFileType);

    const RamProject *project() const;

    QString customSettings() const;
    void setCustomSettings(const QString &newCustomSettings);

public slots:
    virtual void edit(bool show = true) override;

protected:
    RamPipeFile(QString uuid);

private:
    void construct();

    RamProject *m_project;
};

#endif // RAMPIPEFILE_H
