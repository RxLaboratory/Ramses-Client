#ifndef RAMSEQUENCE_H
#define RAMSEQUENCE_H

#include "ramshot.h"

class RamProject;

class RamSequence : public RamObject
{
    Q_OBJECT
public:
    explicit RamSequence(QString shortName, RamProject *project, QString name,  QString uuid = "");
    ~RamSequence();

    RamProject *project() const;

    static RamSequence *sequence(QString uuid);

public slots:
    void update() override;
    virtual void edit() override;

private:
    RamProject *m_project;
};

#endif // RAMSEQUENCE_H
