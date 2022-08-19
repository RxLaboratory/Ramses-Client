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

    static RamPipe *get(QString uuid);
    static RamPipe *c(RamObject *o);

    // OTHER //

    RamPipe(RamStep *output, RamStep *input);

    RamStep *outputStep() const;
    void setOutputStep(RamObject *outputStep);
    RamStep *inputStep() const;
    void setInputStep(RamObject *inputStep);

    QString name() const override;

    RamProject *project() const;

    RamObjectList *pipeFiles() const;

public slots:
    virtual void edit(bool show = true) override;

protected:
    static QMap<QString, RamPipe*> m_existingObjects;
    RamPipe(QString uuid);
    virtual QString folderPath() const override { return ""; };

private slots:
    // updates the name
    void pipeFileListChanged();

private:
    void construct();
    void getCreateLists();
    void connectEvents();

    RamObjectList *m_pipeFiles;
};

#endif // RAMPIPE_H
