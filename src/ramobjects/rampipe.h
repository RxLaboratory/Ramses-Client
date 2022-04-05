#ifndef RAMPIPE_H
#define RAMPIPE_H

#include "ramstep.h"
#include "data-models/ramobjectlist.h"

class RamProject;

class RamPipe : public RamObject
{
    Q_OBJECT
public:
    RamPipe(RamStep *output, RamStep *input, QString uuid = "");
    ~RamPipe();

    QString name() const override;

    RamStep *outputStep() const;
    void setOutputStep(RamStep *outputStep);

    RamStep *inputStep() const;
    void setInputStep(RamStep *inputStep);

    RamProject *project() const;

    RamObjectList *pipeFiles() const;

    static RamPipe *pipe(QString uuid);

public slots:
    void update() override;
    virtual void edit(bool show = true) override;
    virtual void removeFromDB() override;
    virtual void remove(bool updateDB = true) override;

private slots:
    void pipeFileUnassigned(const QModelIndex &parent, int first, int last);
    void pipeFileAssigned(const QModelIndex &parent, int first, int last);
    void pipeFileUnassigned();
    void pipeFileChanged();

private:
    RamStep *m_outputStep;
    RamStep *m_inputStep;
    RamObjectList *m_pipeFiles;
    QMetaObject::Connection m_inputConnection;
    QMetaObject::Connection m_outputConnection;
};

#endif // RAMPIPE_H
