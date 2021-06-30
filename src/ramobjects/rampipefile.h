#ifndef RAMPIPEFILE_H
#define RAMPIPEFILE_H

#include <QObject>

#include "ramfiletype.h"

class RamProject;

class RamPipeFile : public RamObject
{
    Q_OBJECT
public:
    explicit RamPipeFile(QString shortName, RamProject *project);
    /**
     * @brief RamPipeFile Creqtes qn empty pipefile instqnce. Note that this will NOT creqte it in the database
     * Use RamPipeFile(QString shortName, RamProject *project, QObject *parent = nullptr) to qdd it to the database
     * @param uuid The pipefile uuid
     * @param parent The parent QObject
     */
    explicit RamPipeFile(QString uuid, QObject *parent = nullptr);
    ~RamPipeFile();

    QString name() const override;

    RamFileType *fileType() const;
    void setFileType(RamFileType *newFileType);  

    const RamProject *project() const;
    void setProject(RamProject *project);

    static RamPipeFile *pipeFile(QString uuid);

public slots:
    void update() override;
    virtual void edit(bool show = true) override;

private:
    RamFileType *m_fileType;
    RamProject *m_project;
};

#endif // RAMPIPEFILE_H
