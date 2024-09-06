#ifndef DATASTRUCT_H
#define DATASTRUCT_H

#include <QNetworkRequest>
#include <QSet>

struct ServerConfig {
    QString address = "";
    int updateDelay = 60000;
    int timeout = 3000;
    bool useSsl = true;
    int port = 443;
};

struct Request
{
    QNetworkRequest request;
    QString body;
    QString query;
};

struct TableRow
{
    QString uuid;
    QString data;
    QString role;
    QString modified;
    int removed;
};

struct SyncData
{
    QHash<QString, QSet<TableRow>> tables;
    QHash<QString, QStringList> deletedUuids;
    QString syncDate;
};

struct TableFetchData
{
    QString name;
    int rowCount = 0;
    int pageCount = 0;
    int deleteCount = 0;
    bool pulled = false;
    int currentPage = 0;
};

struct FetchData
{
    QSet<TableFetchData> tables;
    int tableCount;
};

inline bool operator==(const TableFetchData &a, const TableFetchData &b)
{
  return a.name == b.name;
}

inline uint qHash(const TableFetchData &a)
{
    return qHash(a.name);
}

inline bool operator==(const TableRow &a, const TableRow &b)
{
  return a.uuid == b.uuid;
}

inline uint qHash(const TableRow &a)
{
    return qHash(a.uuid);
}

#endif // DATASTRUCT_H
