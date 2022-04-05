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

    bool canExportFileType(RamFileType *ft) const;
    bool canExportFileType(QString extension) const;
    bool canImportFileType(RamFileType *ft) const;
    bool canImportFileType(QString extension) const;

    RamObjectList *nativeFileTypes() const;
    RamObjectList *importFileTypes() const;
    RamObjectList *exportFileTypes() const;

    bool canOpen(QString filePath) const;
    bool open(QString filePath) const;

    static RamApplication *application(QString uuid);

public slots:
    void unassignFileType(RamObject *o);
    void update() override;
    virtual void edit(bool show = true) override;
    virtual void removeFromDB() override;
    virtual void remove(bool updateDB = true) override;

private slots:
    void nativeFileTypeUnassigned(const QModelIndex &parent, int first, int last);
    void nativeFileTypeAssigned(const QModelIndex &parent, int first, int last);
    void importFileTypeUnassigned(const QModelIndex &parent, int first, int last);
    void importFileTypeAssigned(const QModelIndex &parent, int first, int last);
    void exportFileTypeUnassigned(const QModelIndex &parent, int first, int last);
    void exportFileTypeAssigned(const QModelIndex &parent, int first, int last);

private:
    QString m_executableFilePath;
    RamObjectList *m_nativeFileTypes;
    RamObjectList *m_importFileTypes;
    RamObjectList *m_exportFileTypes;
};

#endif // RAMAPPLICATION_H
