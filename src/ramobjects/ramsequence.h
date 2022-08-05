#ifndef RAMSEQUENCE_H
#define RAMSEQUENCE_H

#include "ramobject.h"

template<typename RO> class RamObjectFilterModel;
class RamProject;
class RamShot;

class RamSequence : public RamObject
{
    Q_OBJECT
public:

    // STATIC METHODS //

    static RamSequence *getObject(QString uuid, bool constructNew = false);

    // METHODS //

    RamSequence(QString shortName, QString name, RamProject *project);

    int shotCount() const;
    double duration() const;

    RamProject *project() const;

    virtual QString details() const override;

public slots:
    virtual void edit(bool show = true) override;

protected:
    RamSequence(QString uuid);

private:
    void construct();
    void setProject(RamProject *project);

    RamProject *m_project;
    RamObjectFilterModel<RamShot*> *m_shots;
};

#endif // RAMSEQUENCE_H
