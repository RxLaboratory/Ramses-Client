#ifndef RAMPROJECT_H
#define RAMPROJECT_H

#include "ramobject.h"

class RamProject : public RamObject
{
    Q_OBJECT
public:
    RamProject(QString shortName, QString name = "", QString uuid = "", QObject *parent = nullptr);
    ~RamProject();

    QString folderPath() const;
    void setFolderPath(const QString &folderPath);

    void update();

private:
    QString _folderPath;

};

#endif // RAMPROJECT_H
