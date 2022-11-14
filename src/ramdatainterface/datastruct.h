#ifndef DATASTRUCT_H
#define DATASTRUCT_H

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
    QString userName;
    QString modified;
    int removed;
};

struct SyncData
{
    QHash<QString, QSet<TableRow>> tables;
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

#endif // DATASTRUCT_H
