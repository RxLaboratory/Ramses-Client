#ifndef RAMITEMTABLELISTPROXY_H
#define RAMITEMTABLELISTPROXY_H

#include <QIdentityProxyModel>

class RamItemTableListProxy : public QIdentityProxyModel
{
    Q_OBJECT
public:
    RamItemTableListProxy(QObject *parent = nullptr);

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
};

#endif // RAMITEMTABLELISTPROXY_H
