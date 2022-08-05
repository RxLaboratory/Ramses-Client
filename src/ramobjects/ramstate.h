#ifndef RAMSTATE_H
#define RAMSTATE_H

#include "ramobject.h"

#include <QColor>

class RamState : public RamObject
{
    Q_OBJECT
public:

    // STATIC METHODS //

    static RamState *getObject(QString uuid, bool constructNew = false);

    // METHODS //

    RamState(QString shortName, QString name);

    int completionRatio() const;
    void setCompletionRatio(int completionRatio);

    virtual QString details() const override;

protected:
    RamState(QString uuid);

public slots:
    virtual void edit(bool show = true) override;

private:
    void construct();

};

#endif // RAMSTATE_H
