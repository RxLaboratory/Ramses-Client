#ifndef TIMELINEPROXY_H
#define TIMELINEPROXY_H

#include "duutils/qtransposeproxymodel.h"

class TimeLineProxy : public QTransposeProxyModel
{
public:
    TimeLineProxy(QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
};

#endif // TIMELINEPROXY_H
