#ifndef RAMAPPLICATION_H
#define RAMAPPLICATION_H

#include "ramobject.h"
#include "ramfiletype.h"
#include "data-models/ramobjectlist.h"

class RamApplication : public RamObject
{
    Q_OBJECT
public:
    RamApplication(QString shortName, QString name = "", QString executableFilePath = "", QString uuid = "");
    ~RamApplication();

    QString executableFilePath() const;
    void setExecutableFilePath(const QString &executableFilePath);

    void update() override;

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
    virtual void edit(bool show = true) override;

private slots:
    void nativeFileTypeUnassigned(const QModelIndex &parent, int first, int last);
    void nativeFileTypeAssigned(const QModelIndex &parent, int first, int last);
    void importFileTypeUnassigned(const QModelIndex &parent, int first, int last);
    void importFileTypeAssigned(const QModelIndex &parent, int first, int last);
    void exportFileTypeUnassigned(const QModelIndex &parent, int first, int last);
    void exportFileTypeAssigned(const QModelIndex &parent, int first, int last);

private:
    QString _executableFilePath;
    RamObjectList *m_nativeFileTypes;
    RamObjectList *m_importFileTypes;
    RamObjectList *m_exportFileTypes;
};

#endif // RAMAPPLICATION_H
