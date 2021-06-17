#ifndef RAMAPPLICATION_H
#define RAMAPPLICATION_H

#include "ramobject.h"
#include "ramfiletype.h"
#include "ramobjectlist.h"

class RamApplication : public RamObject
{
    Q_OBJECT
public:
    RamApplication(QString shortName, QString name = "", QString executableFilePath = "", QString uuid = "", QObject *parent = nullptr);
    ~RamApplication();

    QString executableFilePath() const;
    void setExecutableFilePath(const QString &executableFilePath);

    void update();

    bool canExportFileType(RamFileType *ft) const;
    bool canExportFileType(QString extension) const;
    bool canImportFileType(RamFileType *ft) const;
    bool canImportFileType(QString extension) const;

    RamObjectList *nativeFileTypes() const;
    RamObjectList *importFileTypes() const;
    RamObjectList *exportFileTypes() const;

    static RamApplication *application(QString uuid);

public slots:
    void unassignFileType(RamObject *o);

private slots:
    void nativeFileTypeUnassigned(RamObject *ft);
    void nativeFileTypeAssigned(RamObject *const ft);
    void importFileTypeUnassigned(RamObject *ft);
    void importFileTypeAssigned(RamObject *const ft);
    void exportFileTypeUnassigned(RamObject *ft);
    void exportFileTypeAssigned(RamObject *const ft);

private:
    QString _executableFilePath;
    RamObjectList *_nativeFileTypes;
    RamObjectList *_importFileTypes;
    RamObjectList *_exportFileTypes;
};

#endif // RAMAPPLICATION_H
