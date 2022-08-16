#ifndef RAMPIPEFILE_H
#define RAMPIPEFILE_H

#include <QObject>

#include "ramobject.h"
#include "ramproject.h"
#include "ramfiletype.h"

class RamPipeFile : public RamObject
{
    Q_OBJECT
public:

    // STATIC //

    static RamPipeFile *get(QString uuid);
    static RamPipeFile *c(RamObject *o);

    // OTHER //

    RamPipeFile(QString shortName, RamProject *project);
    RamPipeFile(QString uuid);

    RamFileType *fileType() const;
    void setFileType(RamFileType *newFileType);

    const RamProject *project() const;

    QString customSettings() const;
    void setCustomSettings(const QString &newCustomSettings);

    virtual QString details() const override;

public slots:
    virtual void edit(bool show = true) override;

private:
    void construct();

    RamProject *m_project;
};

#endif // RAMPIPEFILE_H
