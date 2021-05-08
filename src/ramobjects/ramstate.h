#ifndef RAMSTATE_H
#define RAMSTATE_H

#include "ramobject.h"

#include <QColor>

class RamState : public RamObject
{
    Q_OBJECT
public:
    explicit RamState(QString shortName, QString name = "", QString uuid = "", QObject *parent = nullptr);
    ~RamState();

    QColor color() const;
    void setColor(const QColor &color);

    int completionRatio() const;
    void setCompletionRatio(int completionRatio);

    void update();

private:
    QColor _color;
    int _completionRatio = 50;
};

#endif // RAMSTATE_H