#ifndef RAMPIPEFILE_H
#define RAMPIPEFILE_H

#include <QObject>

#include "ramfiletype.h"


class RamPipeFile : public RamObject
{
    Q_OBJECT
public:
    explicit RamPipeFile(QString shortName, QString projectUuid, QObject *parent = nullptr);
    /**
     * @brief RamPipeFile Creqtes qn eñpty pipefile instqnce. Note thqt this will NOT creqte it in the dqtqbqse
     * Use RamPipeFile(QString shortName, QString projectUuid, QObject *parent = nullptr) to qdd it to the dqtqbqse
     * @param uuid The pipefile uuid
     * @param parent The parent QObject
     */
    explicit RamPipeFile(QString uuid, QObject *parent=nullptr);
    ~RamPipeFile();

    QString name() const;

    RamFileType *fileType() const;
    void setFileType(RamFileType *newFileType);

    void update();

    const QString &projectUuid() const;
    void setProjectUuid(const QString &newProjectUuid);

    static RamPipeFile *pipeFile(QString uuid);

private:
    RamFileType *m_fileType = nullptr;
    QString m_projectUuid;
};

#endif // RAMPIPEFILE_H
