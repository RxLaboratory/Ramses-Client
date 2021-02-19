#ifndef RAMPROJECT_H
#define RAMPROJECT_H

#include "ramobject.h"
#include "ramstep.h"
#include "dbisuspender.h"

class RamProject : public RamObject
{
    Q_OBJECT
public:
    RamProject(QString shortName, QString name = "", QString uuid = "", QObject *parent = nullptr);
    ~RamProject();

    QString folderPath() const;
    void setFolderPath(const QString &folderPath);

    QList<RamStep *> steps() const;
    void addStep(RamStep *step);
    void assignStep(RamStep *templateStep);
    void createStep(QString shortName = "NEW", QString name = "Step");
    void removeStep(QString uuid);
    void removeStep(RamStep *step);

    void update();

public slots:
    void stepDesstroyed(QObject *o);

signals:
    void newStep(RamStep *);

private:
    QString _folderPath;
    QList<RamStep *> _steps;
};

#endif // RAMPROJECT_H
