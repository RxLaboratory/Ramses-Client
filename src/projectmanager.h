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

    QStringList recentProjects() const;

public slots:
    void setProject(const QString &projectPath);
    void closeProject();

signals:
    void projectChanged(const QString &);

private:
    static ProjectManager *_instance;
    explicit ProjectManager(QObject *parent = nullptr);

    QString _currentProject;

    // Helpers
    void addToRecentList(const QString &projectPath);
};

#endif // PROJECTMANAGER_H
