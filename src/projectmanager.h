#ifndef PROJECTMANAGER_H
#define PROJECTMANAGER_H

#include <QObject>

/**
 * @brief The ProjectManager class is a singleton used to manage projects:
 * Open/Create/Join/Close/Edit a project.
 */
class ProjectManager : public QObject
{
    Q_OBJECT
public:
    static ProjectManager *i();

    QStringList recentDatabases() const;

public slots:
    void openDatabase(const QString &dbPath);
    void loadDatabase(const QString &dbPath);
    void closeDatabase();
    bool createDatabase(const QString &filePath, const QString &projectWorkingFolder);

signals:
    void databaseChanged(const QString &);

private:
    static ProjectManager *_instance;
    explicit ProjectManager(QObject *parent = nullptr);

    QString _currentProject;

    // Helpers
    void addToRecentList(const QString &projectPath);
};

#endif // PROJECTMANAGER_H
