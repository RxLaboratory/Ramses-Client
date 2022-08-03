#ifndef RAMAPPLICATION_H
#define RAMAPPLICATION_H

#include "ramobject.h"

template<typename RO> class RamObjectList;
class RamFileType;

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

    // OTHER //

    /**
     * @brief RamApplication creates a new Application in the database
     * @param shortName
     * @param name
     */
    RamApplication(QString shortName, QString name);

    QString executableFilePath() const;
    void setExecutableFilePath(const QString &executableFilePath);

    RamObjectList<RamFileType *> *nativeFileTypes() const;
    RamObjectList<RamFileType *> *importFileTypes() const;
    RamObjectList<RamFileType *> *exportFileTypes() const;

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

    RamObjectList<RamFileType*> *m_nativeFileTypes;
    RamObjectList<RamFileType*> *m_importFileTypes;
    RamObjectList<RamFileType*> *m_exportFileTypes;
};

#endif // RAMAPPLICATION_H
