#ifndef DBSYNC_H
#define DBSYNC_H

#include <QObject>

class DBSync : public QObject
{
    Q_OBJECT
public:
    explicit DBSync(QObject *parent = nullptr);

signals:

};

#endif // DBSYNC_H
