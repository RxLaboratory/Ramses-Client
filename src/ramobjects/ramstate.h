#ifndef RAMSTATE_H
#define RAMSTATE_H

#include "ramobject.h"

class RamState : public RamObject
{
    Q_OBJECT
public:

    // STATIC METHODS //

    static RamState *get(QString uuid);
    static RamState *c(RamObject *o);

    // METHODS //

    RamState(QString shortName, QString name);
    RamState(QString uuid);

    int completionRatio() const;
    void setCompletionRatio(int completionRatio);

    virtual QString details() const override;

public slots:
    virtual void edit(bool show = true) override;

private:
    void construct();

};

#endif // RAMSTATE_H
