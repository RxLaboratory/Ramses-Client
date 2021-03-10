#ifndef RAMSEQUENCE_H
#define RAMSEQUENCE_H

#include "ramobject.h"

class RamSequence : public RamObject
{
    Q_OBJECT
public:
    explicit RamSequence(QString shortName, QString name, QString projectUuid,  QString uuid = "", QObject *parent = nullptr);
    ~RamSequence();

    QString projectUuid() const;
    void setProjectUuid(const QString &projectUuid);

    void update();

private:
    QString _projectUuid;
    //QList<RamAsset*> _assets;
};

#endif // RAMSEQUENCE_H
