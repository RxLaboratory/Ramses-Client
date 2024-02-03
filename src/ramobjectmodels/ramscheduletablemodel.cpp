#include "ramscheduletablemodel.h"

#include "duqf-app/app-config.h"
#include "ramschedulecomment.h"
#include "ramscheduleentry.h"
#include "ramses.h"

RamScheduleTableModel::RamScheduleTableModel(QObject *parent) : QAbstractTableModel(parent)
{
    m_startDate = QDate::currentDate().addDays(-5);
    m_endDate = QDate::currentDate();
}

void RamScheduleTableModel::setObjectModel(RamObjectModel *users, DBTableModel *schedule, DBTableModel *comments)
{
    beginResetModel();

    if (m_users) disconnect(m_users, nullptr, this, nullptr);
    if (m_schedule) disconnect(m_schedule, nullptr, this, nullptr);
    if (m_comments) disconnect(m_comments, nullptr, this, nullptr);

    m_users = users;
    m_schedule = schedule;
    m_comments = comments;

    if (m_users)
    {
        connect( m_users, &RamObjectModel::rowsInserted, this, &RamScheduleTableModel::removeUser);
        connect( m_users, &RamObjectModel::rowsAboutToBeRemoved, this, &RamScheduleTableModel::insertUser);
        connect( m_users, &RamObjectModel::dataChanged, this, &RamScheduleTableModel::changeUser);
        connect( m_users, &RamObjectModel::modelReset, this, &RamScheduleTableModel::resetUsers);
    }

    if (m_schedule)
    {
        connect( m_schedule, &DBTableModel::rowsInserted, this,&RamScheduleTableModel::removeSchedule);
        connect( m_schedule, &DBTableModel::rowsAboutToBeRemoved, this, &RamScheduleTableModel::insertSchedule);
        connect( m_schedule, &DBTableModel::dataChanged, this, &RamScheduleTableModel::changeSchedule);
        connect( m_schedule, &DBTableModel::modelReset, this, &RamScheduleTableModel::resetSchedule);
    }

    if (m_comments)
    {
        connect( m_comments, &DBTableModel::rowsInserted, this, &RamScheduleTableModel::insertComment);
        connect( m_comments, &DBTableModel::rowsAboutToBeRemoved, this, &RamScheduleTableModel::removeComment);
        connect( m_comments, &DBTableModel::dataChanged, this, &RamScheduleTableModel::changeComment);
        connect( m_comments, &DBTableModel::modelReset, this, &RamScheduleTableModel::resetComments);
    }   

    endResetModel();
}

int RamScheduleTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    if (!m_schedule) return 0;

    return m_startDate.daysTo( m_endDate ) + 1;
}

int RamScheduleTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    int c = 0;

    if (!m_users && !m_comments) return c;

    // Two rows per user: AM and PM
    if (m_users) c = m_users->rowCount()*2;
    // If there are comments, it's on an additionnal row
    if (m_comments) c++;

    return c;
}

QVariant RamScheduleTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (!m_users) return QVariant();

    if (orientation == Qt::Vertical)
    {
        // Invalid index
        if (section < 0 || section > m_users->rowCount()*2)
        {
            //if (role == Qt::DisplayRole) return "Invalid";
            return QVariant();
        }

        // General
        if (role == Qt::TextAlignmentRole) return Qt::AlignCenter;

        // The comments
        if (section == 0 && m_comments) {
            if ( role == Qt::DisplayRole )
                return "NOTES";
            if ( role == Qt::ToolTipRole )
                return QString("Use this line to add comments for specific dates.");
            if ( role == RamObject::IsPM )
                return false;
            return QAbstractTableModel::headerData(section, orientation, role);
        }

        if (m_comments) section--;
        RamObject *usrObj = m_users->get(section / 2);

        // Add AMP/PM Info to user data

        QString ampm = "AM";
        if (section % 2 == 1) ampm = "PM";

        if ( role == RamObject::IsPM)
            return section % 2 == 1;

        if ( role == Qt::DisplayRole )
            return QString(usrObj->name() % "\n" % ampm);

        if ( role == Qt::ToolTipRole )
            return QString(usrObj->roleData(role).toString() % " | " % ampm);

        if ( role == Qt::StatusTipRole )
            return QString(usrObj->roleData(role).toString() % " | " % ampm);

        // Or return default

        return usrObj->roleData(role);
    }
    else
    {
        QDate date = m_startDate.addDays(section);

        if ( role == Qt::DisplayRole )
        {
            return QString(date.toString("MMMM\ndddd\nyyyy-MM-dd"));
        }

        if (role == Qt::ForegroundRole)
        {
            if (date == QDate::currentDate())
                return QBrush(QColor(213,136,241));

            RamProject *proj = Ramses::instance()->currentProject();
            if (proj)
            {
                if (date == proj->deadline()) return QBrush(QColor(249,105,105));
            }

            if (date.weekNumber() % 2 == 1)
                return QBrush(QColor(130,130,130));
            else
                return QBrush(QColor(170,170,170));
        }

        if (role ==  RamObject::Date)
            return date;
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}

QVariant RamScheduleTableModel::data(const QModelIndex &index, int role) const
{
    if (!m_users) return QVariant();

    int row = index.row();
    int col = index.column();

#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    QDateTime date = QDateTime( m_startDate.addDays(col) );
#else
    QDateTime date = QDate( m_startDate.addDays(col) ).startOfDay();
#endif

    // THE DATE
    QString ampm = "am";
    if ( headerData(row, Qt::Vertical,  RamObject::IsPM).toBool())
    {
        ampm = "pm";
        date.setTime(QTime(12,0));
    }

    if (role ==  RamObject::Date )
    {
        return date;
    }

    // THE COMMENT
    if (row == 0 && m_comments) {
        QSet<RamObject*> comments = m_comments->lookUp("date", date.toString(DATETIME_DATA_FORMAT));
        if (!comments.isEmpty())
        {
            RamObject *c = *comments.cbegin();
            return c->roleData(role);
        }
        /*for(int i = 0; i < m_comments->rowCount(); i++)
        {
            RamScheduleComment *c = RamScheduleComment::c(m_comments->get(i));
            if (!c) continue;
            if (c->date() == date)
                return c->roleData(role);
        }*/

        // Needed Default values

        if (role == Qt::BackgroundRole )
        {
            if (date.date().weekNumber() % 2 == 1)
                return QColor(51,51,51);
            else
                return QColor(42,42,42);
        }

        if (role == RamObject::IsComment) return true;

        return QVariant();
    }

    // THE ENTRY

    // Filters
    QString usrUuid = m_users->getUuid((row-1) / 2);
    if (usrUuid == "") return QVariant();

    QSet<RamObject*> entries = m_schedule->lookUp("date", date.toString( DATETIME_DATA_FORMAT ));
    RamScheduleEntry *entry = nullptr;
    foreach(RamObject *entryObj, entries) {
        RamScheduleEntry *e = RamScheduleEntry::c( entryObj );
        if (!e) continue;
        if (!e->user()) continue;
        if (e->user()->uuid() == usrUuid) {
            entry = e;
            break;
        }
    }

    if (entry)
    {
        // Add AMP/PM Info to entry data
        if ( role == RamObject::IsPM)
            return ampm == "pm";

        if ( role == Qt::ToolTipRole )
            return QString(entry->roleData(role).toString() % "\n" % ampm);

        if ( role == Qt::StatusTipRole )
            return QString(entry->roleData(role).toString() % " | " % ampm);

        // Or return default
        return entry->roleData(role);
    }

    if (role == Qt::EditRole) {
        return "";
    }

    if (role == Qt::BackgroundRole )
    {
        if (date.date().weekNumber() % 2 == 1)
            return QBrush(QColor(51,51,51));
        else
            return QBrush(QColor(42,42,42));
    }

    return QVariant();
}

void RamScheduleTableModel::changeUser(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
    if (!m_users) return;
    int first = topLeft.row();
    int last = bottomRight.row();

    // 2 rows per user
    first *= 2;
    last *= 2;
    // A comment row
    if (m_comments) {
        first++;
        last++;
    }
    // Last is PM
    last++;

    QModelIndex tl = index(first, 0);
    QModelIndex br = index(last, columnCount());

    emit headerDataChanged(Qt::Vertical, first, last);
    emit dataChanged(tl, br, roles);
}

void RamScheduleTableModel::insertUser(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent);

    if (!m_users) return;

    //We're inserting new rows
    beginInsertRows(QModelIndex(), first*2, last*2+1);
    // Finished!
    endInsertRows();
}

void RamScheduleTableModel::removeUser(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent);

    if (!m_users) return;

    // We're removing rows
    beginRemoveRows(QModelIndex(), first*2, last*2+1);
    endRemoveRows();
}

void RamScheduleTableModel::resetUsers()
{
    beginResetModel();
    endResetModel();
}

void RamScheduleTableModel::resetSchedule()
{
    beginResetModel();
    endResetModel();
}

void RamScheduleTableModel::changeSchedule(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
    if (!m_schedule) return;
    int first = topLeft.row();
    int last = bottomRight.row();
    for (int i = first; i <= last; i++)
    {
        RamScheduleEntry *e = RamScheduleEntry::c( m_schedule->get(i) );
        QModelIndex ind = entryIndex(e);
        emit dataChanged(ind, ind, roles);
    }
}

void RamScheduleTableModel::insertSchedule(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent);

    if (!m_schedule) return;
    for (int i = first; i <= last; i++)
    {
        RamScheduleEntry *e = RamScheduleEntry::c( m_schedule->get(i) );
        QModelIndex ind = entryIndex(e);
        emit dataChanged(ind, ind, QVector<int>());
    }
}

void RamScheduleTableModel::removeSchedule(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent);

    if (!m_schedule) return;
    for (int i = first; i <= last; i++)
    {
        RamScheduleEntry *e = RamScheduleEntry::c( m_schedule->get(i) );
        QModelIndex ind = entryIndex(e);
        emit dataChanged(ind, ind, QVector<int>());
    }
}

void RamScheduleTableModel::changeComment(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
    if (!m_comments) return;

    int first = topLeft.row();
    int last = bottomRight.row();

    for (int i = first; i <= last; i++)
    {
        RamScheduleComment *c = RamScheduleComment::c( m_comments->get(i) );
        if (!c) continue;
        int col = colForDate(c->date());
        QModelIndex ind = this->index(0, col);
        emit dataChanged(ind, ind, roles);
    }
}

void RamScheduleTableModel::insertComment(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent);

    if (!m_comments) return;

    for (int i = first; i <= last; i++)
    {
        RamScheduleComment *c = RamScheduleComment::c( m_comments->get(i) );
        if (!c) continue;
        int col = colForDate(c->date());
        QModelIndex ind = this->index(0, col);
        emit dataChanged(ind, ind);
    }
}

void RamScheduleTableModel::removeComment(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent);

    if (!m_comments) return;

    for (int i = first; i <= last; i++)
    {
        RamScheduleComment *c = RamScheduleComment::c( m_comments->get(i) );
        if (!c) continue;
        int col = colForDate(c->date());
        QModelIndex ind = this->index(0, col);
        emit dataChanged(ind, ind);
    }
}

void RamScheduleTableModel::resetComments()
{
    beginResetModel();
    endResetModel();
}

QModelIndex RamScheduleTableModel::entryIndex(RamScheduleEntry *e)
{
    if (!e) return QModelIndex();
    // Get the user to get the row
    RamUser *u = e->user();
    if (!u) return QModelIndex();
    int row = m_users->objectRow(u);
    // Get the column from the date
    QDateTime d = e->date();
    int col = colForDate(d);
    // Adjust user row
    // x2 (2 rows per user)
    // +1 (comments is the first)
    // +1 if pm
    row *= 2;
    if (m_comments) row++;
    if (d.time().hour() >= 12) row++;
    return this->index(row, col);
}

int RamScheduleTableModel::colForDate(QDateTime date)
{
    return m_startDate.daysTo(date.date());
}

void RamScheduleTableModel::setEndDate(const QDate &newEndDate)
{
    if (newEndDate < m_startDate) return;

    if (newEndDate > m_endDate)
    {
        beginInsertColumns(QModelIndex(), columnCount(), columnCount() + m_endDate.daysTo(newEndDate) -1);
        m_endDate = newEndDate;
        endInsertColumns();
    }
    else if (newEndDate < m_endDate)
    {
        beginRemoveColumns(QModelIndex(), columnCount() - newEndDate.daysTo(m_endDate) , columnCount() -1);
        m_endDate = newEndDate;
        endRemoveColumns();
    }
}

void RamScheduleTableModel::setStartDate(const QDate &newStartDate)
{
    if (newStartDate > m_endDate) return;

    if (newStartDate < m_startDate)
    {
        beginInsertColumns(QModelIndex(), 0, newStartDate.daysTo(m_startDate)-1);
        m_startDate = newStartDate;
        endInsertColumns();
    }
    else if (newStartDate > m_startDate)
    {
        beginRemoveColumns(QModelIndex(), 0, m_startDate.daysTo(newStartDate) -1);
        m_startDate = newStartDate;
        endRemoveColumns();
    }
}

