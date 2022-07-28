#ifndef RAMSTATE_H
#define RAMSTATE_H

#include "ramobject.h"

#include <QColor>

class RamState : public RamObject
{
    Q_OBJECT
public:
    explicit RamState(QString shortName, QString name = "", QString uuid = "");
    ~RamState();

    QColor color() const;
    void setColor(const QColor &color);

    int completionRatio() const;
    void setCompletionRatio(int completionRatio);

    static RamState *state(QString uuid);

public slots:
    virtual void edit(bool show = true) override;

private:
    QColor _color;
    int _completionRatio = 50;
};

#endif // RAMSTATE_H
