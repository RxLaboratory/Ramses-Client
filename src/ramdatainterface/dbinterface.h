#ifndef DBINTERFACE_H
#define DBINTERFACE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSettings>
#include <QNetworkCookieJar>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDeadlineTimer>
#include <QApplication>
#include <QStringBuilder>
#include <QTimer>
#include <QThread>
#include <QProgressBar>
#include <QVersionNumber>

#include "duutils/utils.h"
#include "duutils/duqflogger.h"
#include "localdatainterface.h"

class DBInterface : public DuQFLoggerObject
{
    Q_OBJECT
public:

    // ==== STATIC UTILS ====

    static QStringList recentDatabases();
    static void addToRecentList(const QString &dbFile);
    static bool isTeamProject(const QString &dbFile);

    /**
     * @brief i Gets the unique DBInterface i.
     * @return the unique i.
     */
    static DBInterface *i();

    bool isSyncSuspended();
    bool isTeamProject();

    // DATA INTERFACE //

    QSet<QString> tableUuids(QString table);
    bool contains(QString uuid, QString table, bool includeRemoved = false);

    void createObject(QString uuid, QString table, QString data);

    QString objectData(QString uuid, QString table);
    void setObjectData(QString uuid, QString table, QString data);
    /**
     * @brief validateObjectData Checks if the given data is a correct JSON format or tries to fix it
     * @return The validated/fixed data or an empty string if it's invalid.
     */
    QString validateObjectData(QString data, QString uuid = "", QString type = "", bool ignoreErrors = false);

    void removeObject(QString uuid, QString table);
    void restoreObject(QString uuid, QString table);
    bool isRemoved(QString uuid, QString table);

    QString modificationDate(QString uuid, QString table);

    const QString &dataFile() const;
    bool loadDataFile(const QString &file);

    QString getUserRole(const QString &uuid);
    bool setUserRole(const QString &uuid, const QString role);

    QString lastError() const { return m_lastError; }

signals:
    void databaseReady();
    void syncFinished();
    void syncStarted();

public slots:
    void suspendSync();
    void resumeSync();
    bool sync();
    bool fullSync();

    // MAINTENANCE //
    QString cleanDabaBase(int deleteDataOlderThan = -1);
    bool undoClean();
    void acceptClean();

protected:
    /**
     * @brief The unique DBInteface instance
     */
    static DBInterface *_instance;

private slots:
    /**
     * @brief finishSync is called when the LDI has finished saving sync. Emits syncFinished and Schedules the next autosync.
     */
    void finishSync();

private:
    /**
     * @brief This is a singleton, private constructor
     * @param parent
     */
    explicit DBInterface(QObject *parent = nullptr);
    /**
     * @brief Connects all member events (just at the end of constructor method)
     */
    void connectEvents();
    /**
     * @brief m_ldi is the interface with the local data.
     */
    LocalDataInterface *m_ldi;
    /**
     * @brief m_serverAddress The address of the server
     */
    QString m_serverAddress;
    /**
     * @brief m_updateFrequency 60s by default
     */
    int m_updateFrequency = 60000;
    QTimer *m_updateTimer;
    bool m_syncSuspended = true;

    QString m_lastError;
};

#endif // DBINTERFACE_H
