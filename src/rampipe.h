#ifndef RAMPIPE_H
#define RAMPIPE_H

#include "ramstep.h"

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

    RamFileType *fileType() const;
    void setFileType(RamFileType *fileType);

public slots:
    void removeFileType();

private:
    RamStep *_outputStep;
    RamStep *_inputStep;
    QString _projectUuid;
    RamFileType *_fileType;
    QMetaObject::Connection _inputConnection;
    QMetaObject::Connection _outputConnection;
    QMetaObject::Connection _fileTypeConnection;
};

#endif // RAMPIPE_H
