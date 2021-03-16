#ifndef RAMAPPLICATION_H
#define RAMAPPLICATION_H

#include "ramobject.h"

class RamApplication : public RamObject
{
    Q_OBJECT
public:
    RamApplication(QString shortName, QString name = "", QString executableFilePath = "", QString uuid = "", QObject *parent = nullptr);
    ~RamApplication();

    QString executableFilePath() const;
    void setExecutableFilePath(const QString &executableFilePath);

    void update();

private:
    QString _executableFilePath;

};

#endif // RAMAPPLICATION_H
