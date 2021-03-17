#ifndef RAMAPPLICATION_H
#define RAMAPPLICATION_H

#include "ramobject.h"
#include "ramfiletype.h"

class RamApplication : public RamObject
{
    Q_OBJECT
public:
    RamApplication(QString shortName, QString name = "", QString executableFilePath = "", QString uuid = "", QObject *parent = nullptr);
    ~RamApplication();

    QString executableFilePath() const;
    void setExecutableFilePath(const QString &executableFilePath);

    void update();

    void unassignFileType(QString uuid);

    bool canExportFileType(RamFileType *ft) const;
    bool canExportFileType(QString extension) const;
    bool canImportFileType(RamFileType *ft) const;
    bool canImportFileType(QString extension) const;

    QList<RamFileType *> nativeFileTypes() const;
    void clearNativeFileTypes();
    void assignNativeFileType(RamFileType *const ft);
    void unassignNativeFileType(RamFileType *ft);
    void unassignNativeFileType(QString uuid);

    QList<RamFileType *> importFileTypes() const;
    void clearImportFileTypes();
    void assignImportFileType(RamFileType * const ft);
    void unassignImportFileType(RamFileType *ft);
    void unassignImportFileType(QString uuid);

    QList<RamFileType *> exportFileTypes() const;
    void clearExportFileTypes();
    void assignExportFileType(RamFileType * const ft);
    void unassignExportFileType(RamFileType *ft);
    void unassignExportFileType(QString uuid);

public slots:
    void unassignFileType(RamObject *o);

signals:
    void fileTypeAssigned(RamFileType*);
    void nativeFileTypeAssigned(RamFileType*);
    void importFileTypeAssigned(RamFileType*);
    void exportFileTypeAssigned(RamFileType*);
    void fileTypeUnassigned(QString);
    void nativeFileTypeUnassigned(QString);
    void importFileTypeUnassigned(QString);
    void exportFileTypeUnassigned(QString);

private:
    QString _executableFilePath;
    QList<RamFileType*> _nativeFileTypes;
    QList<RamFileType*> _importFileTypes;
    QList<RamFileType*> _exportFileTypes;
};

#endif // RAMAPPLICATION_H
