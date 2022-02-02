#include "timelinedelegate.h"

#include "duqf-app/app-style.h"
#include "ramshot.h"

TimelineDelegate::TimelineDelegate(QObject *parent)
    : RamObjectDelegate{parent}
{

}

QSize TimelineDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)

    // Reinterpret the int to a pointer
    quintptr iptr = index.data(Qt::UserRole).toULongLong();

    if (iptr == 0) return QSize(250, 30);

    RamShot *shot = reinterpret_cast<RamShot*>(iptr);
    if (!shot) return QSize(250, 30);

    return QSize(shot->duration()*50, 30);
}
