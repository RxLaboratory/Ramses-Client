#ifndef RAMPIPE_H
#define RAMPIPE_H

#include "ramstep.h"
#include "ramobjectlist.h"

class RamPipe : public RamObject
{
    Q_OBJECT
public:
    RamPipe(RamStep *output, RamStep *input, QString uuid = "", QObject *parent = nullptr);
    ~RamPipe();

    void update();

    RamStep *outputStep() const;
    void setOutputStep(RamStep *outputStep);

    RamStep *inputStep() const;
    void setInputStep(RamStep *inputStep);

    QString projectUuid() const;
    void setProjectUuid(const QString &projectUuid);

    RamObjectList *pipeFiles() const;

public slots:

private slots:
    void pipeFileUnassigned(RamObject *ft);
    void pipeFileAssigned(RamObject *const ft);

private:
    RamStep *_outputStep;
    RamStep *_inputStep;
    QString _projectUuid;
    RamObjectList *m_pipeFiles;
    QMetaObject::Connection _inputConnection;
    QMetaObject::Connection _outputConnection;
};

#endif // RAMPIPE_H
