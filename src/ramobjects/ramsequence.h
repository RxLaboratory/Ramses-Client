#ifndef RAMSEQUENCE_H
#define RAMSEQUENCE_H

#include "ramshot.h"
#include "ramobjectlist.h"

class RamProject;

class RamSequence : public RamObjectList
{
    Q_OBJECT
public:
    explicit RamSequence(QString shortName, RamProject *project, QString name,  QString uuid = "", QObject *parent = nullptr);
    ~RamSequence();

    RamProject *project() const;

    // Shots
    void append(RamShot *shot);
    void createShot(QString shortName = "NEW", QString name = "Shot");

    static RamSequence *sequence(QString uuid);

public slots:
    void update() Q_DECL_OVERRIDE;
    void remove() Q_DECL_OVERRIDE;

private:
    RamProject *m_project;
};

#endif // RAMSEQUENCE_H
