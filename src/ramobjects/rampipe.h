#ifndef RAMPIPE_H
#define RAMPIPE_H

#include "ramstep.h"
#include "ramobjectlist.h"

class RamProject;

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

    RamProject *project() const;

    RamObjectList *pipeFiles() const;

    static RamPipe *pipe(QString uuid);

public slots:

private slots:
    void pipeFileUnassigned(RamObject *ft);
    void pipeFileAssigned(RamObject *const ft);

private:
    RamStep *m_outputStep;
    RamStep *m_inputStep;
    RamObjectList *m_pipeFiles;
    QMetaObject::Connection m_inputConnection;
    QMetaObject::Connection m_outputConnection;
};

#endif // RAMPIPE_H
