#ifndef RAMPIPE_H
#define RAMPIPE_H

#include "ramobject.h"

#include "ramproject.h"
class RamObjectList;
class RamStep;

class RamPipe : public RamObject
{
    Q_OBJECT
public:

    // STATIC //

    static RamPipe *getObject(QString uuid, bool constructNew = false);
    static RamPipe *c(RamObject *o);

    // OTHER //

    RamPipe(RamStep *output, RamStep *input);

    RamStep *outputStep() const;
    void setOutputStep(RamStep *outputStep);
    RamStep *inputStep() const;
    void setInputStep(RamStep *inputStep);

    RamProject *project() const;

    RamObjectList *pipeFiles() const;

public slots:
    virtual void edit(bool show = true) override;

protected:
    RamPipe(QString uuid);

private slots:
    // updates the name
    void pipeFileListChanged();

private:
    void construct();
    void connectEvents();

    RamObjectList *m_pipeFiles;
};

#endif // RAMPIPE_H
