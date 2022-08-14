#ifndef RAMAPPLICATION_H
#define RAMAPPLICATION_H

#include "ramobject.h"

#include "data-models/ramobjectlist.h"
#include "ramfiletype.h"

class RamApplication : public RamObject
{
    Q_OBJECT
public:

    // STATIC //

    /**
     * @brief application return an existing Application
     * @param uuid
     * @return
     */
    static RamApplication *getObject(QString uuid, bool constructNew = false);
    static RamApplication *c(RamObject *o);

    // OTHER //

    /**
     * @brief RamApplication creates a new Application in the database
     * @param shortName
     * @param name
     */
    RamApplication(QString shortName, QString name);

    QString executableFilePath() const;
    void setExecutableFilePath(const QString &executableFilePath);

    RamObjectList *nativeFileTypes() const;
    RamObjectList *importFileTypes() const;
    RamObjectList *exportFileTypes() const;

    bool canExportFileType(RamFileType *ft) const;
    bool canExportFileType(QString extension) const;
    bool canImportFileType(RamFileType *ft) const;
    bool canImportFileType(QString extension) const;

    bool canOpen(QString filePath) const;
    bool open(QString filePath) const;

public slots:
    void unassignFileType(RamFileType *ft);
    virtual void edit(bool show = true) override;

protected:
    /**
     * @brief RamApplication constructs a RamApplication from the database
     * @param uuid
     */
    RamApplication(QString uuid);

private:
    void construct();

    RamObjectList *m_nativeFileTypes;
    RamObjectList *m_importFileTypes;
    RamObjectList *m_exportFileTypes;
};

#endif // RAMAPPLICATION_H
