#ifndef RAMWORKINGFOLDER_H
#define RAMWORKINGFOLDER_H

#include <QObject>

/**
 * @brief The RamFolder class represents a working folder in the Ramses tree,
 * with its versions, published and preview folder
 */
class RamWorkingFolder
{
public:
    /**
     * @brief RamFolder Constructor
     * @param path The path to the folder
     */
    explicit RamWorkingFolder(QString path);
    const QString &path() const;

    /**
     * @brief isPublished checks if the file has been published
     * @param version is the specific version which has to be published. If 0, will look for *any* version.
     * @param resource checks a specific resource
     * @return true if the file is published
     */
    bool isPublished(int version = 0, QString resource = "") const;
    /**
     * @brief defaultWorkingFile The default file (the one without resource string)
     * @return The path to the file, an empty string if not found
     */
    QString defaultWorkingFile() const;
    /**
     * @brief resourceFiles Gets the available working files
     * @return The list of paths to the files
     */
    QStringList workingFiles() const;
    /**
     * @brief publishedFiles Gets the latest published files
     * @param resource The resource to get
     * @param version A specific version, or 0 for the latest one
     * @return The file paths
     */
    QStringList publishedFiles(QString resource = "", int version = 0) const;

    // <=== Path Getters ===>

    /**
     * @brief publishPath gets the paths for published resources
     * @return
     */
    QString publishPath() const;
    /**
     * @brief previewPath gets the path for preview files
     * @return
     */
    QString previewPath() const;
    /**
     * @brief versionsPath gets the path for version history
     * @return
     */
    QString versionsPath() const;


private:
    QString m_path;

    QStringList listFiles(QString folderPath) const;
};

#endif // RAMWORKINGFOLDER_H
