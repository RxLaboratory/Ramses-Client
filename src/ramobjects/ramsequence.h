#ifndef RAMSEQUENCE_H
#define RAMSEQUENCE_H

#include "ramobject.h"

class RamObjectFilterModel;
class RamProject;

class RamSequence : public RamObject
{
    Q_OBJECT
public:

    // STATIC METHODS //

    static RamSequence *getObject(QString uuid, bool constructNew = false);

    // METHODS //

    RamSequence(QString shortName, QString name, RamProject *project);

    QColor color() const;
    void setColor(const QColor &newColor);

    int shotCount() const;
    double duration() const;

    RamProject *project() const;

public slots:
    virtual void edit(bool show = true) override;

protected:
    RamSequence(QString uuid);

private:
    void construct();
    void setProject(RamProject *project);

    RamProject *m_project;
    RamObjectFilterModel *m_shots;
};

#endif // RAMSEQUENCE_H
